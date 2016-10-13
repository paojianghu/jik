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


#ifndef CORE_LAYER_DROPOUT_H_
#define CORE_LAYER_DROPOUT_H_


#include <core/layer.h>
#include <core/log.h>
#include <memory>
#include <limits>
#include <random>
#include <vector>


namespace jik {


/*!
 *  \class  LayerDropout
 *  \brief  Dropout
 */
template <typename Dtype>
class LayerDropout: public Layer<Dtype> {
  // Public types
 public:
  typedef Dtype         Type;
  typedef Layer<Dtype>  Parent;


  // Protected attributes
 protected:
  Dtype                       drop_prob_;   // Probability to drop
  std::shared_ptr<Mat<Dtype>> mask_;        // Mask


  // Public methods
 public:
  /*!
   * Constructor.
   *
   *  \param[in]  name : layer name
   *  \param[in]  in   : input activations
   *  \param[in]  param: parameters
   */
  LayerDropout(const char*                                     name,
               const std::vector<std::shared_ptr<Mat<Dtype>>>& in,
               const Param&                                    param):
    Parent(name, in) {
    // Make sure we have 1 input
    Check(Parent::in_.size() == 1, "Layer '%s' must have 1 input",
          Parent::Name());

    // Probability to drop
    param.Get("drop_prob", &drop_prob_);

    // Create the mask and initialize it to 0
    mask_ = std::make_shared<Mat<Dtype>>(Parent::in_[0]->size);

    // Create 1 output, same size as the input
    Parent::out_.resize(1);
    Parent::out_[0] = std::make_shared<Mat<Dtype>>(Parent::in_[0]->size);
  }

  /*!
   * Destructor.
   */
  virtual ~LayerDropout() {}

  /*!
   * Clear the derivatives.
   */
  virtual void ClearDeriv() {
    Parent::ClearDeriv();
    mask_->deriv->Zero();
  }

  /*!
   * Forward pass.
   * The forward pass calculates the outputs activations
   * in regard to the inputs activations and weights.
   *
   *  \param[in]  state: state
   */
  virtual void Forward(const State& state) {
    if (state.phase != State::PHASE_TRAIN) {
      // Dropout only during the training phase
      Parent::out_[0]->data = Parent::in_[0]->data;
      return;
    }

    // out = mask * in
    if (drop_prob_ < std::numeric_limits<Dtype>::epsilon()) {
      // Nothing to drop: just copy the input to the output
      Parent::out_[0]->data = Parent::in_[0]->data;
      mask_->Set(static_cast<Dtype>(1));
    } else if (drop_prob_ > static_cast<Dtype>(1) -
               std::numeric_limits<Dtype>::epsilon()) {
      // Drop everything: zero out the data
      Parent::out_[0]->Zero();
      mask_->Zero();
    } else {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_real_distribution<Dtype> dist(static_cast<Dtype>(0),
                                                 static_cast<Dtype>(1));

      Dtype* mask_data = mask_->Data();
      Dtype scale      = static_cast<Dtype>(1) /
                         (static_cast<Dtype>(1) - drop_prob_);
      for (uint32_t i = 0; i < mask_->Size(); ++i) {
        if (dist(gen) < drop_prob_) {
          mask_data[i] = static_cast<Dtype>(0);
        } else {
          mask_data[i] = scale;
        }
      }

      Dtype*       out_data  = Parent::out_[0]->Data();
      const Dtype* in_data   = Parent::in_[0]->Data();
      for (uint32_t i = 0; i < Parent::out_[0]->Size(); ++i) {
        out_data[i] = mask_data[i] * in_data[i];
      }
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
    const Dtype* out_deriv_data  = Parent::out_[0]->DerivData();
    const Dtype* in_data         = Parent::in_[0]->Data();
    const Dtype* mask_data       = mask_->Data();
    Dtype*       in_deriv_data   = Parent::in_[0]->DerivData();
    Dtype*       mask_deriv_data = mask_->DerivData();

    // in_deriv   = mask * out_deriv
    // mask_deriv = in * out_deriv
    for (uint32_t i = 0; i < Parent::out_[0]->Size(); ++i) {
      Dtype dv            = out_deriv_data[i];
      in_deriv_data[i]   += mask_data[i] * dv;
      mask_deriv_data[i] += in_data[i]   * dv;
    }
  }
};


}  // namespace jik


#endif  // CORE_LAYER_DROPOUT_H_