#include <program_lua.hpp>

#include <catch2/catch_test_macros.hpp>
#include <spdlog/spdlog.h>

using namespace std::literals;

TEST_CASE("lua program: window blind complete run", "[single-file]") {
  spdlog::set_level(spdlog::level::debug);

  HomeAutomation::GV gv;
  gv.inputs["input_up"] = false;
  gv.inputs["input_down"] = false;
  gv.outputs["output_up"] = false;
  gv.outputs["output_down"] = false;

  auto path = std::string{"test/test_program_with_blind.lua"};
  auto luaProgram = HomeAutomation::Runtime::LuaProgram(path, &gv);
  auto start = std::chrono::high_resolution_clock::now();
  auto const periodIdle = 500ms;
  auto const periodUp = 30s;
  auto const periodDown = 30s;

  // idle
  auto const timestamp_start_idle = start + periodIdle + 100ms;
  gv.inputs["input_up"] = false;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_idle);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == false);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);

  // up
  gv.inputs["input_up"] = true;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_idle + 100ms);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == true);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);

  gv.inputs["input_up"] = true;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_idle + 200ms);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == true);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);

  gv.inputs["input_up"] = false;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_idle + 300ms);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == true);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);

  // idle
  auto const timestamp_start_idle_2 = start + periodIdle + 500ms;
  gv.inputs["input_up"] = true;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_idle_2);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == false);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);

  gv.inputs["input_up"] = true;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_idle_2 + 100ms);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == false);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);

  gv.inputs["input_up"] = false;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_idle_2 + 200ms);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == false);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);

  gv.inputs["input_up"] = true;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_idle_2 + 300ms);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == false);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);

  // down
  auto const timestamp_start_down =
      start + periodIdle + 500ms + periodIdle + 100ms;

  gv.inputs["input_up"] = false;
  gv.inputs["input_down"] = true;
  luaProgram.execute(timestamp_start_down + 300ms);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == false);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == true);

  gv.inputs["input_up"] = true;
  gv.inputs["input_down"] = false;
  luaProgram.execute(timestamp_start_down + periodDown + 100ms);
  REQUIRE(std::get<bool>(gv.outputs["output_up"]) == false);
  REQUIRE(std::get<bool>(gv.outputs["output_down"]) == false);
}