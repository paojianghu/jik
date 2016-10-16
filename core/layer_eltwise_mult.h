/*!
  The MIT License (MIT)

  Copyright (c)2016 Olivier Soares

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
 */


#ifndef CORE_LAYER_ELTWISE_MULT_H_
#define CORE_LAYER_ELTWISE_MULT_H_


#include <core/layer.h>
#include <core/log.h>
#include <memory>
#include <vector>


namespace jik {


/*!
 *  \class  LayerEltwiseMult
 *  \brief  Element wise multiplication (Hadamard product)
 */
template <typename Dtype>
class LayerEltwiseMult: public Layer<Dtype> {
  // Public types
 public:
  typedef Dtype         Type;
  typedef Layer<Dtype>  Parent;


  // Public methods
 public:
  /*!
   * Constructor.
   *
   *  \param[in]  name: layer name
   *  \param[in]  in  : input activations
   */
  LayerEltwiseMult(const char*                                     name,
                   const std::vector<std::shared_ptr<Mat<Dtype>>>& in):
    Parent(name, in) {
    // Make sure we have 2 inputs and they have the same size
    Check(Parent::in_.size() == 2, "Layer '%s' must have 2 inputs",
          Parent::Name());
    Check(Parent::in_[0]->Size() == Parent::in_[1]->Size(),
          "Layer '%s' inputs must have the same size", Parent::Name());

    // Create 1 output, same size as the inputs
    Parent::out_.resize(1);
    Parent::out_[0] = std::make_shared<Mat<Dtype>>(Parent::in_[0]->size);
  }

  /*!
   * Forward pass.
   * The forward pass calculates the outputs activations
   * in regard to the inputs activations and weights.
   *
   *  \param[in]  state: state
   */
  virtual void Forward(const State& state) {
    Dtype*       out_data = Parent::out_[0]->Data();
    const Dtype* in1_data = Parent::in_[0]->Data();
    const Dtype* in2_data = Parent::in_[1]->Data();

    // out = in1 . in2 ("." = Hadamard product)
    for (uint32_t i = 0; i < Parent::out_[0]->Size(); ++i) {
      out_data[i] = in1_data[i] * in2_data[i];
    }
  }

  /*!
   * Backward pass.
   * The backward pass calculates the inputs activations and weights
   * derivatives in regard to the outputs activations derivatives.
   *
   *  \param[in]  state: state
   */
  virtual void Backward(const State& state) {
    const Dtype* out_deriv_data = Parent::out_[0]->DerivData();
    const Dtype* in1_data       = Parent::in_[0]->Data();
    const Dtype* in2_data       = Parent::in_[1]->Data();
    Dtype*       in1_deriv_data = Parent::in_[0]->DerivData();
    Dtype*       in2_deriv_data = Parent::in_[1]->DerivData();

    // in1_deriv = in2 * out_deriv
    // in2_deriv = in1 * out_deriv
    for (uint32_t i = 0; i < Parent::out_[0]->Size(); ++i) {
      Dtype dv           = out_deriv_data[i];
      in1_deriv_data[i] += in2_data[i] * dv;
      in2_deriv_data[i] += in1_data[i] * dv;
    }
  }
};


}  // namespace jik


#endif  // CORE_LAYER_ELTWISE_MULT_H_
