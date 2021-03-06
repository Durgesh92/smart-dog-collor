// Copyright 2019 Alpha Cephei Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DURGESH_AI_KALDI_RECOGNIZER_H
#define DURGESH_AI_KALDI_RECOGNIZER_H

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "fstext/fstext-lib.h"
#include "fstext/fstext-utils.h"
#include "decoder/lattice-faster-decoder.h"
#include "feat/feature-mfcc.h"
#include "lat/kaldi-lattice.h"
#include "lat/word-align-lattice.h"
#include "lat/compose-lattice-pruned.h"
#include "nnet3/am-nnet-simple.h"
#include "nnet3/nnet-am-decodable-simple.h"
#include "nnet3/nnet-utils.h"
#include "online2/online-nnet3-wake-word-faster-decoder.h"

#include "online2/online-nnet2-feature-pipeline.h"
#include "nnet3/decodable-online-looped.h"
#include "online2/onlinebin-util.h"
#include "online2/online-timing.h"
#include "online2/online-endpoint.h"
#include "lat/lattice-functions.h"
#include "util/kaldi-thread.h"

#include "online2/online-endpoint.h"

#include "model.h"
#include "spk_model.h"

using namespace kaldi;

enum KaldiRecognizerState {
    RECOGNIZER_INITIALIZED,
    RECOGNIZER_RUNNING,
    RECOGNIZER_ENDPOINT,
    RECOGNIZER_FINALIZED,
    RECOGNIZER_INITIALIZED_ASR,
    RECOGNIZER_RUNNING_ASR,
    RECOGNIZER_ENDPOINT_ASR,
    RECOGNIZER_FINALIZED_ASR
};

class KaldiRecognizer {
    public:
        KaldiRecognizer(Model *model, float sample_frequency);
        KaldiRecognizer(Model *model, SpkModel *spk_model, float sample_frequency);
        KaldiRecognizer(Model *model, float sample_frequency, char const *grammar);
        KaldiRecognizer(Model *model, SpkModel *spk_model, float sample_frequency, char const *grammar);
        KaldiRecognizer(Model *model, float sample_frequency, char const *grammar, bool dummy);
        ~KaldiRecognizer();
        void SetMaxAlternatives(int max_alternatives);
        bool AcceptWaveform(const char *data, int len, bool flag);
        bool AcceptWaveform(const short *sdata, int len, bool flag);
        bool AcceptWaveform(const float *fdata, int len, bool flag);
        bool CheckWakeAvailability(char const *grammar);
        const char* Result();
        const char* FinalResult();
        const char* PartialResult();
        const char* KwsResult();

    private:
        void InitState();
        void InitAsrState();
        void InitRescoring();
        void CleanUp(bool flag);
        void CleanUpAsr();
        void init_asr();
        void UpdateSilenceWeights();
        bool AcceptWaveform(Vector<BaseFloat> &wdata, bool flag);
        bool GetSpkVector(Vector<BaseFloat> &out_xvector, int *frames);
        const char *StoreEmptyReturn();
        const char *GetResult();
        const char *StoreReturn(const string &res);
        const char *MbrResult(CompactLattice &clat);
        const char *NbestResult(CompactLattice &clat);

        Model *model_ = nullptr;

        SingleUtteranceNnet3Decoder *decoder_ = nullptr;

        fst::LookaheadFst<fst::StdArc, int32> *decode_fst_ = nullptr;
        fst::LookaheadFst<fst::StdArc, int32> *decode_fst_asr_ = nullptr;
        fst::StdVectorFst *g_fst_ = nullptr; // dynamically constructed grammar
        OnlineNnet2FeaturePipeline *feature_pipeline_ = nullptr;
        OnlineNnet2FeaturePipeline *feature_pipeline_asr_ = nullptr;
        OnlineSilenceWeighting *silence_weighting_ = nullptr;


        // KWS
        OnlineWakeWordFasterDecoder *wwdecoder_ = nullptr;
        nnet3::DecodableAmNnetLoopedOnline *wwdecodable_ = nullptr;

        // Speaker identification
        SpkModel *spk_model_ = nullptr;
        OnlineBaseFeature *spk_feature_ = nullptr;

        // Rescoring
        fst::ArcMapFst<fst::StdArc, kaldi::LatticeArc, fst::StdToLatticeMapper<kaldi::BaseFloat> > *lm_fst_ = nullptr;

        // RNNLM rescoring
        kaldi::rnnlm::RnnlmComputeStateInfo *info = nullptr;
        fst::ScaleDeterministicOnDemandFst *lm_to_subtract_det_scale = nullptr;
        fst::BackoffDeterministicOnDemandFst<fst::StdArc> *lm_to_subtract_det_backoff = nullptr;
        kaldi::rnnlm::KaldiRnnlmDeterministicFst* lm_to_add_orig = nullptr;
        fst::DeterministicOnDemandFst<fst::StdArc> *lm_to_add = nullptr;


        int max_alternatives_ = 0; // Disable alternatives by default

        float sample_frequency_;
        int32 frame_offset_;

        int64 samples_processed_;
        int64 samples_round_start_;

        int32 wake_word_id_one = -1;
        int32 wake_word_id_two = -1;
        int no_of_wake_words_ = 0;
        int32 unk_id_;

        KaldiRecognizerState state_;
        KaldiRecognizerState state_asr_;
        string last_result_;

        bool do_asr = false;    // used to start asr immediately after kws
        bool is_asr_init = false;
};

#endif /* DURGESH_AI_KALDI_RECOGNIZER_H */
