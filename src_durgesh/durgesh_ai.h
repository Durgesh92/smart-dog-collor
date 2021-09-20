// Copyright 2020-2021 Durgesh
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* This header contains the C API for durgesh_ai speech recognition system */

#ifndef DURGESH_AI_H
#define DURGESH_AI_H

#ifdef __cplusplus
extern "C" {
#endif

/** Model stores all the data required for recognition
 *  it contains static data and can be shared across processing
 *  threads. */
typedef struct Durgesh_aiModel Durgesh_aiModel;


/** Speaker model is the same as model but contains the data
 *  for speaker identification. */
typedef struct Durgesh_aiSpkModel Durgesh_aiSpkModel;


/** Recognizer object is the main object which processes data.
 *  Each recognizer usually runs in own thread and takes audio as input.
 *  Once audio is processed recognizer returns JSON object as a string
 *  which represent decoded information - words, confidences, times, n-best lists,
 *  speaker information and so on */
typedef struct Durgesh_aiRecognizer Durgesh_aiRecognizer;


/** Loads model data from the file and returns the model object
 *
 * @param model_path: the path of the model on the filesystem
 @ @returns model object */
Durgesh_aiModel *durgesh_ai_model_new(const char *model_path,bool flag);


/** Releases the model memory
 *
 *  The model object is reference-counted so if some recognizer
 *  depends on this model, model might still stay alive. When
 *  last recognizer is released, model will be released too. */
void durgesh_ai_model_free(Durgesh_aiModel *model);


/** Check if a word can be recognized by the model
 * @param word: the word
 * @returns the word symbol if @param word exists inside the model
 * or -1 otherwise.
 * Reminding that word symbol 0 is for <epsilon> */
int durgesh_ai_model_find_word(Durgesh_aiModel *model, const char *word);


/** Loads speaker model data from the file and returns the model object
 *
 * @param model_path: the path of the model on the filesystem
 * @returns model object */
Durgesh_aiSpkModel *durgesh_ai_spk_model_new(const char *model_path);


/** Releases the model memory
 *
 *  The model object is reference-counted so if some recognizer
 *  depends on this model, model might still stay alive. When
 *  last recognizer is released, model will be released too. */
void durgesh_ai_spk_model_free(Durgesh_aiSpkModel *model);

/** Creates the recognizer object
 *
 *  The recognizers process the speech and return text using shared model data 
 *  @param sample_rate The sample rate of the audio you going to feed into the recognizer
 *  @returns recognizer object */
Durgesh_aiRecognizer *durgesh_ai_recognizer_new(Durgesh_aiModel *model, float sample_rate);


/** Creates the recognizer object with speaker recognition
 *
 *  With the speaker recognition mode the recognizer not just recognize
 *  text but also return speaker vectors one can use for speaker identification
 *
 *  @param spk_model speaker model for speaker identification
 *  @param sample_rate The sample rate of the audio you going to feed into the recognizer
 *  @returns recognizer object */
Durgesh_aiRecognizer *durgesh_ai_recognizer_new_spk(Durgesh_aiModel *model, Durgesh_aiSpkModel *spk_model, float sample_rate);


/** Creates the recognizer object with the phrase list
 *
 *  Sometimes when you want to improve recognition accuracy and when you don't need
 *  to recognize large vocabulary you can specify a list of phrases to recognize. This
 *  will improve recognizer speed and accuracy but might return [unk] if user said
 *  something different.
 *
 *  Only recognizers with lookahead models support this type of quick configuration.
 *  Precompiled HCLG graph models are not supported.
 *
 *  @param sample_rate The sample rate of the audio you going to feed into the recognizer
 *  @param grammar The string with the list of phrases to recognize as JSON array of strings,
 *                 for example "["one two three four five", "[unk]"]".
 *
 *  @returns recognizer object */
Durgesh_aiRecognizer *durgesh_ai_recognizer_new_wake(Durgesh_aiModel *model, float sample_rate, const char *grammar);

Durgesh_aiRecognizer *durgesh_ai_recognizer_new_gram(Durgesh_aiModel *model, float sample_rate, const char *grammar, bool dummy);

Durgesh_aiRecognizer *durgesh_ai_recognizer_new_grm_spk(Durgesh_aiModel *model,  Durgesh_aiSpkModel *spk_model, float sample_rate, const char *grammar);

/** Accept voice data
 *
 *  accept and process new chunk of voice data
 *
 *  @param data - audio data in PCM 16-bit mono format
 *  @param length - length of the audio data
 *  @returns true if silence is occured and you can retrieve a new utterance with result method */
int durgesh_ai_recognizer_accept_waveform(Durgesh_aiRecognizer *recognizer, const char *data, int length, bool flag);


/** Same as above but the version with the short data for language bindings where you have
 *  audio as array of shorts */
int durgesh_ai_recognizer_accept_waveform_s(Durgesh_aiRecognizer *recognizer, const short *data, int length, bool flag);


/** Same as above but the version with the float data for language bindings where you have
 *  audio as array of floats */
int durgesh_ai_recognizer_accept_waveform_f(Durgesh_aiRecognizer *recognizer, const float *data, int length, bool flag);

bool durgesh_ai_recognizer_check_wakeword_status(Durgesh_aiRecognizer *recognizer, char const *grammar);

bool durgesh_ai_model_check_kws_load_status(Durgesh_aiModel *model);

bool durgesh_ai_model_check_asr_load_status(Durgesh_aiModel *model);

/** Returns speech recognition result
 *
 * @returns the result in JSON format which contains decoded line, decoded
 *          words, times in seconds and confidences. You can parse this result
 *          with any json parser
 *
 * <pre>
 * {
 *   "result" : [{
 *       "conf" : 1.000000,
 *       "end" : 1.110000,
 *       "start" : 0.870000,
 *       "word" : "what"
 *     }, {
 *       "conf" : 1.000000,
 *       "end" : 1.530000,
 *       "start" : 1.110000,
 *       "word" : "zero"
 *     }, {
 *       "conf" : 1.000000,
 *       "end" : 1.950000,
 *       "start" : 1.530000,
 *       "word" : "zero"
 *     }, {
 *       "conf" : 1.000000,
 *       "end" : 2.340000,
 *       "start" : 1.950000,
 *       "word" : "zero"
 *     }, {
 *       "conf" : 1.000000,
 *      "end" : 2.610000,
 *       "start" : 2.340000,
 *       "word" : "one"
 *     }],
 *   "text" : "what zero zero zero one"
 *  }
 * </pre>
 */
const char *durgesh_ai_recognizer_result(Durgesh_aiRecognizer *recognizer);


/** Returns partial speech recognition
 *
 * @returns partial speech recognition text which is not yet finalized.
 *          result may change as recognizer process more data.
 *
 * <pre>
 * {
 *  "partial" : "cyril one eight zero"
 * }
 * </pre>
 */
const char *durgesh_ai_recognizer_partial_result(Durgesh_aiRecognizer *recognizer);

const char *durgesh_ai_recognizer_kws_result(Durgesh_aiRecognizer *recognizer);

/** Returns speech recognition result. Same as result, but doesn't wait for silence
 *  You usually call it in the end of the stream to get final bits of audio. It
 *  flushes the feature pipeline, so all remaining audio chunks got processed.
 *
 *  @returns speech result in JSON format.
 */
const char *durgesh_ai_recognizer_final_result(Durgesh_aiRecognizer *recognizer);


/** Releases recognizer object
 *
 *  Underlying model is also unreferenced and if needed released */
void durgesh_ai_recognizer_free(Durgesh_aiRecognizer *recognizer);

/** Set log level for Kaldi messages
 *
 *  @param log_level the level
 *     0 - default value to print info and error messages but no debug
 *     less than 0 - don't print info messages
 *     greather than 0 - more verbose mode
 */
void durgesh_ai_set_log_level(int log_level);

/**
 *  Init, automatically select a CUDA device and allow multithreading.
 *  Must be called once from the main thread.
 *  Has no effect if HAVE_CUDA flag is not set.
 */
void durgesh_ai_gpu_init();

/**
 *  Init CUDA device in a multi-threaded environment.
 *  Must be called for each thread.
 *  Has no effect if HAVE_CUDA flag is not set.
 */
void durgesh_ai_gpu_thread_init();

#ifdef __cplusplus
}
#endif

#endif /* DURGESH_AI_H */
