#include "TGLRN.h"
#include <bmkernel_api.h>

TGLRN::TGLRN(const onnx::Node &node, MemTable &memTable)
    : TGOperator(node, "LRN"), m_k(1) {

  // TODO
  m_inputAddr = 0;
  m_outputAddr = 0;

  const std::vector<onnx::Dimension> inDim = node.inputs()[0]->sizes();
  m_N = inDim[0].dim;
  m_C = inDim[1].dim;
  m_H = inDim[2].dim;
  m_W = inDim[3].dim;

  m_alpha = node.f(onnx::Symbol("alpha"));
  m_beta = node.f(onnx::Symbol("beta"));
  if (node.hasAttribute(onnx::Symbol("bias"))) {
    m_k = node.f(onnx::Symbol("bias"));
  }
  m_localSize = node.i(onnx::Symbol("size"));
}

void TGLRN::TGLRN::emit(void) const {
  std::cout << "TGLRUN::emit\tm_inputAddr:" << m_inputAddr
            << " m_outputAddr:" << m_outputAddr << " m_N:" << m_N
            << " m_C:" << m_C << " m_H:" << m_H << " m_W:" << m_W
            << " m_alpha:" << m_alpha << "m_localSize:" << m_localSize
            << " m_beta:" << m_beta << "m_k:" << m_k << std::endl;
  bmnet_lrn_forward_bmkernel(m_inputAddr, m_outputAddr, m_N, m_C, m_H, m_W,
                             m_alpha, m_localSize, m_beta, m_k);
}
