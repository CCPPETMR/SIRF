'''
Upper-level demo that illustrates the computation of coil sensitivity maps,
applying projection from the image space into acquisition space and back
defined by the aquisition model, and images and acquisitions algebra.

Usage:
  simple_simulation.py [--help | options]

Options:
  -f <file>, --file=<file>    raw data file
                              [default: simulated_MR_2D_cartesian.h5]
  -e <engn>, --engine=<engn>  reconstruction engine [default: Gadgetron]
  -p <path>, --path=<path>    sub-path to engine module
                              [default: /xGadgetron/pGadgetron]
'''

__version__ = '0.1.0'
from docopt import docopt
args = docopt(__doc__, version=__version__)

import os
import sys

sys.path.append(os.environ.get('SRC_PATH') + args['--path'])

exec('from p' + args['--engine'] + ' import *')

def main():

    # acquisitions will be read from an HDF file
    input_data = AcquisitionData(args['--file'])

    print('---\n acquisition data norm: %e' % input_data.norm())

    # pre-process acquisitions
    print('---\n processing acquisitions...')
    processed_data = preprocess_acquisitions(input_data)

    print('---\n processed acquisition data norm: %e' % processed_data.norm())

    # perform reconstruction
    recon = SimpleReconstruction()
    recon.set_input(processed_data)
    recon.process()
    complex_images = recon.get_output()

    print('---\n reconstructed images norm: %e' % complex_images.norm())

    csms = CoilSensitivityMaps()

    print('---\n sorting acquisitions...')
    processed_data.sort()
    print('---\n computing sensitivity maps...')
    csms.calculate(processed_data)

    # create acquisition model based on the acquisition parameters
    # stored in input_data and image parameters stored in complex_images
    am = AcquisitionModel(processed_data, complex_images)

    am.set_coil_sensitivity_maps(csms)

    # use the acquisition model (forward projection) to produce 'acquisitions'
    acqs = am.forward(complex_images)

    print('---\n reconstructed images forward projection norm %e' % acqs.norm())

    # get data as a Python ndarray
    acqs_data = acqs.as_array();

    # TODO display a slice etc

try:
    main()
    print('done')

except error as err:
    # display error information
    print('??? %s' % err.value)
