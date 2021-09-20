// Copyright 2020 Durgesh Waghmare
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

#ifndef DURGESH_AI_MODEL_H
#define DURGESH_AI_MODEL_H

#include "base/kaldi-common.h"
#include "fstext/fstext-lib.h"
#include "fstext/fstext-utils.h"
#include "online2/onlinebin-util.h"
#include "online2/online-timing.h"
#include "online2/online-endpoint.h"
#include "online2/online-nnet3-decoding.h"
#include "online2/online-feature-pipeline.h"
#include "lat/lattice-functions.h"
#include "lat/sausages.h"
#include "lat/word-align-lattice.h"
#include "lm/const-arpa-lm.h"
#include "util/parse-options.h"
#include "nnet3/nnet-utils.h"
#include "rnnlm/rnnlm-utils.h"
#include "online2/online-nnet3-wake-word-faster-decoder.h"

#include "online2/online-nnet2-feature-pipeline.h"
#include "nnet3/decodable-online-looped.h"
#include "online2/onlinebin-util.h"
#include "online2/online-timing.h"
#include "online2/online-endpoint.h"
#include "fstext/fstext-lib.h"
#include "lat/lattice-functions.h"
#include "util/kaldi-thread.h"
#include "nnet3/nnet-utils.h"
#include "rnnlm/rnnlm-utils.h"
#include "rnnlm/rnnlm-lattice-rescoring.h"

using namespace kaldi;
using namespace std;

class KaldiRecognizer;

class Model {

public:
    Model(const char *model_path);
    // if flag is true use asr model else use kws model
    Model(const char *model_path, bool flag); 
    void Ref();
    void Unref();
    int FindWord(const char *word);
    bool KwsLoadStatus();
    bool AsrLoadStatus();

protected:
    ~Model();
    void ConfigureV1();
    void ConfigureV2();
    void ConfigureWakeWord();
    void ReadDataFiles();
    void WakeWordDataFiles();

    friend class KaldiRecognizer;

    bool model_type_; //true if asr. false if kws

    string model_path_str_;
    string nnet3_rxfilename_;
    string hclg_fst_rxfilename_;
    string hcl_fst_rxfilename_;
    string g_fst_rxfilename_;
    string disambig_rxfilename_;
    string word_syms_rxfilename_;
    string winfo_rxfilename_;
    string carpa_rxfilename_;
    string std_fst_rxfilename_;
    string final_ie_rxfilename_;
    string mfcc_conf_rxfilename_;
    string global_cmvn_stats_rxfilename_;
    string pitch_conf_rxfilename_;


    string rnnlm_word_feats_rxfilename_;
    string rnnlm_feat_embedding_rxfilename_;
    string rnnlm_config_rxfilename_;
    string rnnlm_lm_fst_rxfilename_;
    string rnnlm_lm_rxfilename_;

    kaldi::OnlineEndpointConfig endpoint_config_;
    kaldi::LatticeFasterDecoderConfig nnet3_decoding_config_;
    kaldi::nnet3::NnetSimpleLoopedComputationOptions decodable_opts_;
    kaldi::OnlineNnet2FeaturePipelineInfo feature_info_;

    kaldi::OnlineWakeWordFasterDecoderOpts wake_word_decoder_opts_;

    kaldi::nnet3::DecodableNnetSimpleLoopedInfo *decodable_info_ = nullptr;
    kaldi::TransitionModel *trans_model_ = nullptr;
    kaldi::nnet3::AmNnetSimple *nnet_ = nullptr;
    const fst::SymbolTable *word_syms_ = nullptr;
    bool word_syms_loaded_ = false;
    kaldi::WordBoundaryInfo *winfo_ = nullptr;
    vector<int32> disambig_;

    fst::Fst<fst::StdArc> *hclg_fst_ = nullptr;
    fst::Fst<fst::StdArc> *hcl_fst_ = nullptr;
    fst::Fst<fst::StdArc> *g_fst_ = nullptr;

    fst::Fst<fst::StdArc> *hcl_fst_asr_ = nullptr;
    fst::Fst<fst::StdArc> *g_fst_asr_ = nullptr;

    fst::VectorFst<fst::StdArc> *std_lm_fst_ = nullptr;
    kaldi::ConstArpaLm const_arpa_;

    kaldi::rnnlm::RnnlmComputeStateComputationOptions rnnlm_compute_opts;
    CuMatrix<BaseFloat> word_embedding_mat;
    fst::VectorFst<fst::StdArc> *rnnlm_lm_fst_ = NULL;
    kaldi::nnet3::Nnet rnnlm;

    bool asr_model_load_status_ = false;
    bool kws_model_load_status_ = false;
    int ref_cnt_;
};

#endif /* DURGESH_AI_MODEL_H */
