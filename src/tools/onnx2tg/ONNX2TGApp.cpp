//===- ONNX2TG.cpp --------------------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ONNX2TGApp.h"
#include <cstdlib>
#include <onnc/Target/TargetSelect.h>
#include <onnc/Target/TargetRegistry.h>
#include <onnc/Target/TargetBackend.h>
#include <onnc/Target/TargetOptions.h>
#include <onnc/IRReader/ONNXReader.h>
#include <onnc/IR/Module.h>
#include <onnc/Core/PassManager.h>
#include <onnc/ADT/Color.h>
#include <onnc/Support/IOStream.h>
#include <string>

using namespace onnc;

class TGBackend;
//===----------------------------------------------------------------------===//
// ONNX2TG
//===----------------------------------------------------------------------===//
ONNX2TG::ONNX2TG(int pArgc, char* pArgv[])
  : onnc::CoreApplication(pArgc, pArgv), m_Config() {
  InitializeAllPlatforms();
  InitializeAllBackends();
}

ONNX2TG::~ONNX2TG()
{
}

int ONNX2TG::compile()
{
  onnc::onnx::Reader reader;
  Module module;
  if (!reader.parse(m_Config.input(), module)) {
    return EXIT_FAILURE;
  }

  std::string error;
  std::string quadruple("sophonv1680-bitmain-linux-bmnet-all-0.1.0-none-tg");
  const onnc::Target* target = TargetRegistry::Lookup(quadruple, error);
  if (nullptr == target) {
    errs() << Color::RED << "Error" << Color::RESET
           << ": can not found target `" << quadruple << "`: " << error
           << std::endl;
    return EXIT_FAILURE;
  }

  PassManager pm;
  TargetOptions options;
  TargetBackend* backend = target->createBackend(options);
  backend->addTensorSel(pm);
  backend->addMemAlloc(pm);
  backend->addCodeEmit(pm, m_Config.output());
  pm.run(module);
  return EXIT_SUCCESS;
}