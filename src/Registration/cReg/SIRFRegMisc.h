/*
CCP PETMR Synergistic Image Reconstruction Framework (SIRF)
Copyright 2015 - 2017 Rutherford Appleton Laboratory STFC

This is software developed for the Collaborative Computational
Project in Positron Emission Tomography and Magnetic Resonance imaging
(http://www.ccppetmr.ac.uk/).

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

/*!
\file
\ingroup Registration
\brief Generic tools (e.g., opening files)

\author Richard Brown
\author CCP PETMR
*/

#ifndef _SIRFMISC_H_
#define _SIRFMISC_H_

#include <boost/filesystem.hpp>
#include <nifti1_io.h>

namespace SIRFRegMisc {

    /// Open nifti image
    void open_nifti_image(std::shared_ptr<nifti_image> &image, const boost::filesystem::path filename);

    /// Save nifti image
    void save_nifti_image(std::shared_ptr<nifti_image> image, const std::string filename);

    /// Copy nifti image
    void copy_nifti_image(const std::string input_filename, const std::string output_filename);

    /// Copy nifti image
    void copy_nifti_image(std::shared_ptr<nifti_image> &output_image_sptr, const std::shared_ptr<nifti_image> &image_to_copy_sptr);

    /// Do nifti images match?
    bool do_nift_image_match(const std::shared_ptr<nifti_image> &im1_sptr, const std::shared_ptr<nifti_image> &im2_sptr);

    /// Dump info of nifti image
    void dump_nifti_info(const std::string &im_filename);

    /// Dump info of nifti image
    void dump_nifti_info(const std::shared_ptr<nifti_image> &im1_sptr);

    /// Dump info of multiple nifti images
    void dump_nifti_info(const std::vector<std::shared_ptr<nifti_image> > &ims);

    /// Print info of element of nifti image
    void print_nifti_info(const std::string &im_filename, const std::string keyword);

    /// Print info of element of nifti image
    void print_nifti_info(const std::shared_ptr<nifti_image> &im1_sptr, const std::string keyword);

    /// Save transformation matrix to file
    void save_transformation_matrix(std::shared_ptr<mat44> &transformation_matrix_sptr, const std::string filename);

    /// Read transformation matrix from file
    void open_transformation_matrix(std::shared_ptr<mat44> &transformation_matrix_sptr, const std::string filename);

    /// Print mat44
    void print_mat44(const mat44 *mat_ptr);

    /// Mat44 multiplier
    mat44 multiply_mat44(const mat44 &x, const mat44 &y);
}

#endif
