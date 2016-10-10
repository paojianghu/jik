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


#ifndef CORE_SOLVER_H_
#define CORE_SOLVER_H_


#include <core/model.h>
#include <memory>
#include <cmath>
#include <limits>
#include <vector>
#include <string>


namespace jik {


/*!
 *  \class  Solver
 *  \brief  Base solver
 */
template <typename Dtype>
class Solver {
  // Public types
 public:
  typedef Dtype Type;


  // Protected attributes
 protected:
  std::vector<std::shared_ptr<Mat<Dtype>>>
           weight_;         // List of weights for a model (current value)
  std::vector<std::shared_ptr<Mat<Dtype>>>
           weight_prev_;    // List of weights for a model (previous value)
  uint32_t print_each_;     // Print the model stats every n steps
  uint32_t test_each_;      // Test the model every n steps
  uint32_t save_each_;      // Save the model every n steps
  uint32_t lr_scale_each_;  // Scale the learning rate every n steps
  Dtype    lr_scale_;       // Learning rate scale


  // Public methods
 public:
  /*!
   * Constructor.
   *
   *  \param[in]  print_each   : print the model stats every n steps
   *  \param[in]  test_each    : test the model every n steps
   *  \param[in]  save_each    : save the model every n steps
   *  \param[in]  save_each    : save the model every n steps
   *  \param[in]  lr_scale_each: save the model every n steps
   *  \param[in]  lr_scale     : learning rate scale
   */
  Solver(uint32_t print_each, uint32_t test_each, uint32_t save_each,
         uint32_t lr_scale_each, Dtype lr_scale) {
    print_each_    = print_each;
    test_each_     = test_each;
    save_each_     = save_each;
    lr_scale_each_ = lr_scale_each;
    lr_scale_      = lr_scale;
  }

  /*!
   * Destructor.
   */
  virtual ~Solver() {}

  /*!
   * Learning function.
   *
   *  \param[in]  model        : model
   *  \param[in]  learning_rate: learning rate
   */
  virtual void Learn(Model<Dtype>* model, Dtype learning_rate) const = 0;

  /*!
   * Train a model.
   *
   *  \param[in]  model        : model to train
   *  \param[in]  num_step     : number of training steps
   *  \param[in]  learning_rate: learning rate
   *
   *  \return     Error?
   */
  bool Train(Model<Dtype>* model, uint32_t num_step, Dtype learning_rate) {
    if (!model) {
      Report(kError, "Invalid model");
      return false;
    }

    // Get the model weights and keep track of the previous weights values
    weight_.clear();
    model->GetWeight(&weight_);
    weight_prev_.resize(weight_.size());
    for (size_t i = 0; i < weight_.size(); ++i) {
      weight_prev_[i] = std::make_shared<Mat<Dtype>>(weight_[i]->size, false);
    }

    uint32_t print = 0;
    uint32_t test  = 0;
    uint32_t save  = 0;
    uint32_t lr    = 0;
    for (uint32_t step = 0; step < num_step; ++step) {
      Dtype loss = model->Train();
      Learn(model, learning_rate);

      if ((++print >= print_each_) || (step == num_step - 1)) {
        Report(kInfo, "Step %ld: lr = %f, loss = %f",
               step + 1, learning_rate, loss);
        print = 0;
      }

      if ((++test >= test_each_) || (step == num_step - 1)) {
        Report(kInfo, "Step %ld: accuracy = %f",
               step + 1, model->Test());
        test = 0;
      }

      if ((++save >= save_each_) || (step == num_step - 1)) {
        std::string file_name = model->Name() + std::string("_") +
                                std::to_string(step + 1) + ".model";
        if (!model->Save(file_name.c_str())) {
          return false;
        }
        save = 0;
      }

      if (++lr >= lr_scale_each_) {
        Report(kInfo, "Step %ld: Update learning rate from %f to %f, scale %f",
               step + 1, learning_rate, learning_rate * lr_scale_, lr_scale_);
        learning_rate *= lr_scale_;
        lr = 0;
      }
    }

    // Clear the weights
    weight_.clear();
    weight_prev_.clear();

    return true;
  }
};


}  // namespace jik


#endif  // CORE_SOLVER_H_
