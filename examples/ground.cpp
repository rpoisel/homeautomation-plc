// library
#include <blind.hpp>
#include <light.hpp>

// PLC runtime
#include <gv.hpp>
#include <program.hpp>

#include <spdlog/spdlog.h>

#include <sstream>

// execution context (shall run in dedicated thread with given cycle time)
class GroundLogic final : public HomeAutomation::Runtime::CppProgram {

public:
  GroundLogic(HomeAutomation::GV *gv)
      : CppProgram(gv), stairs_light_trigger{}, kitchen_light_trigger{},
        charger_trigger{}, deck_trigger{}, ground_office_trigger{},
        stairs_light{"Stairs"}, kitchen_light{"Kitchen"}, charger{"Charger"},
        deck_light{"Deck"} {}

  void execute(HomeAutomation::TimeStamp now) override {
    (void)now;

    if (stairs_light_trigger.execute(
            std::get<bool>(gv->inputs["stairs_light"]))) {
      gv->outputs["stairs_light"] = stairs_light.toggle();
    }

    if (kitchen_light_trigger.execute(
            std::get<bool>(gv->inputs["kitchen_light"]))) {
      gv->outputs["kitchen_light"] = kitchen_light.toggle();
    }

    if (charger_trigger.execute(std::get<bool>(gv->inputs["charger"]))) {
      gv->outputs["charger"] = charger.toggle();
    }

    if (deck_trigger.execute(std::get<bool>(gv->inputs["deck_light"]))) {
      gv->outputs["deck_light"] = deck_light.toggle();
    }

    if (u_light_trigger.execute(std::get<bool>(gv->inputs["u_light"]))) {
      gv->outputs["u_light"] = u_light.toggle();
    }

    gv->outputs["ground_office_light"] = ground_office_trigger.execute(
        std::get<bool>(gv->inputs["ground_office_light"]));
  }

private:
  // logic blocks
  HomeAutomation::Library::R_TRIG stairs_light_trigger;
  HomeAutomation::Library::R_TRIG kitchen_light_trigger;
  HomeAutomation::Library::R_TRIG charger_trigger;
  HomeAutomation::Library::R_TRIG deck_trigger;
  HomeAutomation::Library::R_TRIG ground_office_trigger;
  HomeAutomation::Library::R_TRIG u_light_trigger;
  HomeAutomation::Library::Light stairs_light;
  HomeAutomation::Library::Light kitchen_light;
  HomeAutomation::Library::Light charger;
  HomeAutomation::Library::Light deck_light;
  HomeAutomation::Library::Light u_light;
};

namespace HomeAutomation {
namespace Runtime {

std::shared_ptr<HomeAutomation::Runtime::CppProgram>
createCppProgram(std::string const &name, HomeAutomation::GV *gv) {
  if (name == "GroundLogic") {
    return std::make_shared<GroundLogic>(gv);
  }
  std::stringstream s;
  s << "unknown program named " << name << " requested";
  throw std::invalid_argument(s.str());
}

} // namespace Runtime
} // namespace HomeAutomation