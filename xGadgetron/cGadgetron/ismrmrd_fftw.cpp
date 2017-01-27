#include <ismrmrd/ismrmrd.h>
#include <ismrmrd/dataset.h>
#include <ismrmrd/meta.h>
#include <ismrmrd/xml.h>

#include <fftw3.h>

#include "ismrmrd_fftw.h"

namespace ISMRMRD {

#define fftshift(out, in, x, y) circshift(out, in, x, y, (x/2), (y/2))

	int fft2c(NDArray<complex_float_t> &a, bool forward)
	{
		if (a.getNDim() < 2) {
			std::cout << "fft2c Error: input array must have at least two dimensions"
				<< std::endl;
			return -1;
		}

		size_t elements = a.getDims()[0] * a.getDims()[1];
		size_t ffts = a.getNumberOfElements() / elements;

		//Array for transformation
		fftwf_complex* tmp =
			(fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*a.getNumberOfElements());

		if (!tmp) {
			std::cout << "Error allocating temporary storage for FFTW" << std::endl;
			return -1;
		}

		for (size_t f = 0; f < ffts; f++) {

			fftshift(reinterpret_cast<std::complex<float>*>(tmp),
				&a(0, 0, f), a.getDims()[0], a.getDims()[1]);

			//Create the FFTW plan
			fftwf_plan p;
			if (forward) {
				p = fftwf_plan_dft_2d
					(a.getDims()[1], a.getDims()[0], tmp, tmp,
					FFTW_FORWARD, FFTW_ESTIMATE);
			}
			else {
				p = fftwf_plan_dft_2d
					(a.getDims()[1], a.getDims()[0], tmp, tmp,
					FFTW_BACKWARD, FFTW_ESTIMATE);
			}
			fftwf_execute(p);

			fftshift(&a(0, 0, f), reinterpret_cast<std::complex<float>*>(tmp),
				a.getDims()[0], a.getDims()[1]);

			//Clean up.
			fftwf_destroy_plan(p);
		}

		std::complex<float> scale(std::sqrt(1.0f*elements), 0.0);
		for (size_t n = 0; n < a.getNumberOfElements(); n++) {
			a.getDataPtr()[n] /= scale;
		}
		fftwf_free(tmp);
		return 0;
	}

	int fft2c(NDArray<complex_float_t> &a) 
	{
		return fft2c(a, true);
	}

	int ifft2c(NDArray<complex_float_t> &a) 
	{
		return fft2c(a, false);
	}

};

