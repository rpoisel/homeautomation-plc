#include <i2c_bus.hpp>
#include <i2c_dev.hpp>
#include <io_factory.hpp>
#include <scheduler.hpp>

#include <stdexcept>
#include <variant>

namespace HomeAutomation {
namespace Runtime {

struct CopyInstructionInput {
  std::shared_ptr<HomeAutomation::IO::I2C::InputModule> input;
  HomeAutomation::VarValue *value;
  std::uint8_t pin;
};
using CopySequenceInput = std::list<CopyInstructionInput>;
struct CopyInstructionOutput {
  std::shared_ptr<HomeAutomation::IO::I2C::OutputModule> output;
  HomeAutomation::VarValue *value;
  std::uint8_t pin;
};
using CopySequenceOutput = std::list<CopyInstructionOutput>;

class I2CLogic : public HomeAutomation::Scheduler::TaskIOLogic {
public:
  I2CLogic(HomeAutomation::IO::I2C::RealBus &&bus,
           CopySequenceInput &&inputSequence,
           CopySequenceOutput &&outputSequence)
      : bus{std::move(bus)}, inputSequence{std::move(inputSequence)},
        outputSequence{std::move(outputSequence)} {}
  virtual ~I2CLogic() = default;

  void init() override { bus.init(); }

  void shutdown() override { bus.close(); }

  void before() override {
    bus.readInputs();
    for (auto &instr : inputSequence) {
      *instr.value = instr.input->getInput(instr.pin);
    }
  }

  void after() override {
    for (auto &instr : outputSequence) {
      instr.output->setOutput(instr.pin, std::get<bool>(*instr.value));
    }
    bus.writeOutputs();
  }

private:
  HomeAutomation::IO::I2C::RealBus bus;
  CopySequenceInput inputSequence;
  CopySequenceOutput outputSequence;
};

template <typename SequenceType, typename IOType>
static void
insertCopySequence(SequenceType &sequence, HomeAutomation::GvSegment &gvSegment,
                   std::shared_ptr<IOType> io, YAML::Node const &node) {
  for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
    auto const &gvIt = gvSegment.find(it->second.as<std::string>());
    if (gvIt == gvSegment.end()) {
      throw std::invalid_argument("global output variable not found");
    }
    auto &gvVal = gvIt->second;
    if (!std::holds_alternative<bool>(gvVal)) {
      throw std::invalid_argument("unsupported gv variable type");
    }
    uint8_t pin = it->first.as<uint8_t>();
    sequence.emplace_back(io, &gvVal, pin);
  }
}

class IOFactoryI2C {
public:
  static void createIOs(YAML::Node const &ioNode,
                        std::shared_ptr<TaskIOLogicImpl> ioLogic,
                        HomeAutomation::GV &gv) {
    HomeAutomation::IO::I2C::RealBus bus{ioNode["bus"].as<std::string>()};

    CopySequenceInput inputSequence{};
    CopySequenceOutput outputSequence{};

    auto const &componentsNode = ioNode["components"];
    for (YAML::const_iterator componentsIt = componentsNode.begin();
         componentsIt != componentsNode.end(); ++componentsIt) {
      auto const address =
          std::stoul(componentsIt->first.as<std::string>(), nullptr, 16);
      auto const &componentNode = componentsIt->second;

      if (componentNode["direction"].as<std::string>() == "input") {
        std::shared_ptr<HomeAutomation::IO::I2C::InputModule> input;
        if (componentNode["type"].as<std::string>() == "pcf8574") {
          input =
              std::make_shared<HomeAutomation::IO::I2C::PCF8574Input>(address);
        } else {
          throw std::invalid_argument("unknown i2c component type");
        }
        bus.RegisterInput(input);
        insertCopySequence(inputSequence, gv.inputs, input,
                           componentNode["inputs"]);
        // TODO insertCopySequenceInput
      } else if (componentNode["direction"].as<std::string>() == "output") {
        std::shared_ptr<HomeAutomation::IO::I2C::OutputModule> output;
        if (componentNode["type"].as<std::string>() == "pcf8574") {
          output =
              std::make_shared<HomeAutomation::IO::I2C::PCF8574Output>(address);
        } else if (componentNode["type"].as<std::string>() == "max7311") {
          output =
              std::make_shared<HomeAutomation::IO::I2C::MAX7311Output>(address);
        } else {
          throw std::invalid_argument("unknown i2c component type");
        }
        bus.RegisterOutput(output);
        insertCopySequence(outputSequence, gv.outputs, output,
                           componentNode["outputs"]);
      } else {
        throw std::invalid_argument("unknown i2c component direction");
      }
    }

    auto i2cLogic = std::make_shared<I2CLogic>(
        std::move(bus), std::move(inputSequence), std::move(outputSequence));
    ioLogic->addIOSystem(i2cLogic);
  }
};

void IOFactory::createIOs(YAML::Node const &ioNode,
                          std::shared_ptr<TaskIOLogicImpl> ioLogic,
                          HomeAutomation::GV &gv) {
  for (YAML::const_iterator systemIt = ioNode.begin(); systemIt != ioNode.end();
       ++systemIt) {
    auto const &ioEntry = *systemIt;

    auto const &typeNode = ioEntry["type"];
    if (typeNode.as<std::string>() == "i2c") {
      IOFactoryI2C::createIOs(ioEntry, ioLogic, gv);
    } else {
      throw std::invalid_argument("unsupported io type");
    }
  }
}

} // namespace Runtime
} // namespace HomeAutomation