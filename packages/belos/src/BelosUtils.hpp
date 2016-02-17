//@HEADER
// ************************************************************************
//
//                 Belos: Block Linear Solvers Package
//                  Copyright 2004 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
//@HEADER

#ifndef BELOS_UTIL_HPP
#define BELOS_UTIL_HPP
#include "BelosConfigDefs.hpp"

#ifdef HAVE_BELOS_AZTECOO
#include "az_aztec_defs.h" 
#include "Teuchos_ParameterList.hpp"
#include "BelosTypes.hpp"
using namespace std;

namespace Belos{
 enum XLateStatus {
    OK    =0x00,
    WARN  =0x01,
    ERROR =0x02};

std::pair< std::string, int >  
Belos_Translate_from_Aztec_Params(const int * aztec_options, 
				  const double * aztec_params,
				  Teuchos::ParameterList &tpl ) {
 
  int econd = OK;
  ostringstream error;
  if(aztec_options == NULL || aztec_params == NULL ) {
    return std::pair<std::string,int>(string("Belos_Translate_from_Aztec_Params:: Aztec Options or Parameters were null."),econd);
  }

  switch (aztec_options[AZ_solver]){
  case AZ_gmres:

    tpl.set("Solver Name","Pseudoblock GMRES");
    break;
  case AZ_cg:
  case AZ_cg_condnum:
    tpl.set("Solver Name","Pseudoblock CG");
    break; 
  case AZ_lu:
    tpl.set("Solver Name","Klu2");
    break;
  case AZ_bicgstab:
    tpl.set("Solver Name","BICGSTAB");
    break;
  case AZ_tfqmr:
    tpl.set("Solver Name","TFQMR");
    break;
  case AZ_cgs:
    error<<" Translate_Params_Aztec_to_Belos:: uncaught solver name  AZ_cgs"<<std::endl;
    econd |= ERROR;
    break;
  case AZ_slu:
    error<<" Translate_Params_Aztec_to_Belos:: uncaught solver name  AZ_slu"<<std::endl;
    econd |= ERROR;
    break;
  case AZ_symmlq:
    error<<" Translate_Params_Aztec_to_Belos:: uncaught solver name  AZ_symmlq"<<std::endl;
    econd |= ERROR;
    break;
  case AZ_GMRESR:
    error<<" Translate_Params_Aztec_to_Belos:: uncaught solver name  AZ_GMRESR"<<std::endl;
    econd |= ERROR;
    break;
  case AZ_fixed_pt:
    error<<" Translate_Params_Aztec_to_Belos:: uncaught solver name  AZ_fixed_pt"<<std::endl;
    econd |= ERROR;
    break;
  case AZ_analyze:
    error<<" Translate_Params_Aztec_to_Belos:: uncaught solver name  AZ_analyze "<<std::endl;
    econd |= ERROR;
    break;
  case AZ_gmres_condnum:
    error<<" Translate_Params_Aztec_to_Belos:: uncaught solver name  AZ_gmres_condnum."<<std::endl;
    econd |= ERROR;
    break;

  default:
    error << "Translate_Params_Aztec_to_Belos:: unknown solver enum "<<aztec_options[AZ_solver]<<std::endl;
    econd |= ERROR;
  }
  // sierra
  //PRECONDITIONING METHOD=DD-ICC
  //PRECONDITIONING METHOD=DD-ILU
  //PRECONDITIONING METHOD=DD-ILUT

  switch (aztec_options[AZ_precond]) {
  case AZ_none:
    break; // only valid option.
  case AZ_sym_GS: 
  case AZ_ls:
  case AZ_Jacobi:
  case AZ_Neumann:
  case AZ_dom_decomp:
  default:
    error<<" Belos does not have built in preconditioners, Az_precond ignored."<<std::endl;
    econd |= WARN;
  };

  switch(aztec_options[AZ_subdomain_solve]) {
  case AZ_none:
    break; // only valid option
  case AZ_lu:
  case AZ_ilut:
  case AZ_ilu:
  case AZ_rilu:
  case AZ_bilu:
  case AZ_icc:
  default:
      error<<" Belos does not have built in subdomain solvers, Az_subdomain_solve ignored."<<std::endl;
    econd |= WARN;
  }

  // All sierra options are here.
  switch(aztec_options[AZ_conv]) {
  case AZ_r0:
    tpl.set("Implicit Residual Scaling","Norm of Initial Residual");
    break;
  case AZ_rhs:
    tpl.set("Implicit Residual Scaling","Norm of RHS");
    break;
  case AZ_Anorm:
    tpl.set("Implicit Residual Scaling","Norm of Preconditioned Initial Residual");
    break;
  case AZ_noscaled:
    tpl.set("Implicit Residual Scaling","None");
    break;
  case AZ_sol:   
  case AZ_expected_values:
  default: 
    error << "Belos_Translate_from_Aztec_Params: AZ_conv of AZ_sol or AZ_expected_values are not valid for belos. "<<std::endl;
    econd |= ERROR;
  }

  // Make Belos produce output like AztecOO's.
  tpl.set("Output Style", static_cast<int> (Belos::Brief));
  // Always print Belos' errors.  You can add the 
  // enum values together to get all of their effects.
  Belos::MsgType belosPrintOptions = static_cast<Belos::MsgType> (static_cast<int> (Belos::Errors) + static_cast<int> (Belos::Warnings));

  switch(aztec_options[AZ_output]) {
    //  "Errors",
    //     "Warnings",
    //     "IterationDetails",
    //     "OrthoDetails",
    //     "FinalSummary",
    //     "TimingDetails",
    //     "StatusTestDetails",
    //     "Debug"
    
  case AZ_none:
    tpl.set("Output Frequency", -1);
    break;
  case AZ_all:
    tpl.set("Output Frequency", 1);
    belosPrintOptions = static_cast<Belos::MsgType> (static_cast<int> (belosPrintOptions) + static_cast<int> (Belos::StatusTestDetails));
    belosPrintOptions = static_cast<Belos::MsgType> (static_cast<int> (belosPrintOptions) + static_cast<int> (Belos::FinalSummary));
    break;
  case AZ_warnings: // only print warnings
    tpl.set("Output Frequency", -1);
    break;
  case AZ_last:	// only print the final result
    tpl.set("Output Frequency", -1);
    belosPrintOptions = static_cast<Belos::MsgType> (static_cast<int> (belosPrintOptions) + static_cast<int> (Belos::FinalSummary));
    break;
  default: // some integer; print every that many iterations
    // This is an int and not a enum, so I don't need to cast it.
    const int freq = aztec_options[AZ_output];
    tpl.set("Output Frequency", freq);
    belosPrintOptions = static_cast<Belos::MsgType> (static_cast<int> (belosPrintOptions) + static_cast<int> (Belos::StatusTestDetails));
    belosPrintOptions = static_cast<Belos::MsgType> (static_cast<int> (belosPrintOptions) + static_cast<int> (Belos::FinalSummary));
    break;
  }
  tpl.set("Verbosity", static_cast<int> (belosPrintOptions));

  // Only set the "Orthogonalization" parameter if using GMRES.  CG
  // doesn't accept that parameter.
  // sierra uses only ORTHOG METHOD=CLASSICAL
  if (aztec_options[AZ_solver] == AZ_gmres) {
    switch(aztec_options[AZ_orthog]) {
    case AZ_classic:
      tpl.set("Orthogonalization","ICGS");
      break;
    case AZ_modified:
      tpl.set("Orthogonalization","IMGS");
      break;
    default:
      error<<"Option AZ_orthog for GMRES not recognized "<<aztec_options[AZ_orthog]<<endl;
      econd |= ERROR;
      // no way to set DGKS
    }
  }

  if(aztec_options[AZ_max_iter]!=0)   
    tpl.set("Maximum Iterations",aztec_options[AZ_max_iter]);

 // Only set the "Num Blocks" (restart length) parameter if using
  // GMRES.  CG doesn't accept that parameter.
  if (aztec_options[AZ_solver] == AZ_gmres && 
      aztec_options[AZ_kspace] !=0) {
    tpl.set("Num Blocks",aztec_options[AZ_kspace]);
  }

  // aztec_params tested, only AZ_tol should be set. 
  tpl.set("Convergence Tolerance",aztec_params[AZ_tol]);
  for(int i=AZ_drop ; i<= AZ_weights ; ++i) {
    if(aztec_params[i]!=0 ){
      error << " Aztec_Params at "<<i<<" non zero and will be ignored"<<std::endl;
      econd |= WARN;
    }
  }

 

  return std::pair<std::string,int>(error.str(),econd);
}
}
#endif
#endif
