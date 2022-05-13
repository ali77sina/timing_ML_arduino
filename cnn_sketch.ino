// Importing libraries 
#include "C:\Users\lina3953\Documents\Arduino\libraries\Arduino_TensorFlowLite\src/model_conv_N2_student.h"
#include "TensorFlowLite.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"    //includes micro operations in the model, not used in this script!
#include "tensorflow/lite/micro/micro_error_reporter.h"   //error log used with the interpreter 
#include "tensorflow/lite/micro/micro_interpreter.h"    //the interpreter for the tflit model
#include "tensorflow/lite/micro/all_ops_resolver.h"   //includes all of the operations currently supported by the tflite library 

//student model takes 175ms on nano
//student model takes 7.76 ms on teensy
#include "tensorflow/lite/version.h"

// TFLite globals, used for compatibility with Arduino-style sketches
namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;
  TfLiteTensor* model_output = nullptr;

  // Create an area of memory to use for input, output, and other TensorFlow
  // arrays. You'll need to adjust this by combiling, running, and looking
  // for errors.
  constexpr int kTensorArenaSize = 60 * 1024;    //5Kb
  uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setup() {
  pinMode(2, OUTPUT);
  // put your setup code here, to run once:
  // Set up logging (will report to Serial, even within TFLite functions)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure
  model = tflite::GetModel(model_conv_N2_student);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report("Model version does not match Schema");
    while(1);
  }

  // Pulling in all the operations tflite currently supports
  // it is more efficient to make an object of micro_mutable_op_resolver and pull only required operators based on your model
  static tflite::AllOpsResolver resolver;
  

  // Build an interpreter to run the model
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize,
    error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    while(1);
  }

  // Assign model input and output buffers (tensors) to pointers
  model_input = interpreter->input(0);
  model_output = interpreter->output(0);
}

void loop() {
  // put your main code here, to run repeatedly:



//cnn 2 5 order filter 8 hidden 1 output ---> 460 us
//mlp 20 input 8 hidden 1 output ---> 110-120 us

//int t = micros();


// Run inference
for (int i=0;i<504;i++){
model_input->data.f[i] = analogRead(A0);
}
digitalWrite(2, HIGH);
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed on input");
  }
  // Read the predictions from the mode's output buffer (tensor)
  float y_val = model_output->data.f[0];
  //TF_LITE_REPORT_ERROR(error_reporter, y_val);
  //Serial.println( micros() - t);
  digitalWrite(2, LOW);
}
