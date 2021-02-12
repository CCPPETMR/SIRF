function p = examples_data_path(petmr)
% If SIRF_PATH is defined, returns path to PET or MR examples raw data.
% Otherwise, defaults to current folder.

% SyneRBI Synergistic Image Reconstruction Framework (SIRF).
% Copyright 2015 - 2017 Rutherford Appleton Laboratory STFC.
% 
% This is software developed for the Collaborative Computational
% Project in Synergistic Reconstruction for Biomedical Imaging (formerly CCP PETMR)
% (http://www.ccpsynerbi.ac.uk/).
% 
% Licensed under the Apache License, Version 2.0 (the "License");
% you may not use this file except in compliance with the License.
% You may obtain a copy of the License at
% http://www.apache.org/licenses/LICENSE-2.0
% Unless required by applicable law or agreed to in writing, software
% distributed under the License is distributed on an "AS IS" BASIS,
% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
% See the License for the specific language governing permissions and
% limitations under the License.

    
    SIRF_INSTALL_PATH = getenv('SIRF_INSTALL_PATH');
    SIRF_DATA_PATH = getenv('SIRF_DATA_PATH')
    if ~isempty(SIRF_DATA_PATH)
        p = [ SIRF_DATA_PATH filesep 'examples' filesep petmr];
    elseif ~isempty(SIRF_PATH)
        % p = [SIRF_PATH '/data/examples/' petmr];
        p = [ SIRF_INSTALL_PATH filesep 'share' filesep 'SIRF' filesep 'data' filesep 'examples' filesep petmr ];
    else
        p = pwd;
    end
end