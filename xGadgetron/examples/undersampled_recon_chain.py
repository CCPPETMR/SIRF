'''
Lower-level demo, 2-chain GRAPPA reconstruction of undersampled data.

Usage:
  undersampled_recon_chain.py [--help | options]

Options:
  -f <file>, --file=<file>    raw data file
                              [default: simulated_MR_2D_cartesian_Grappa2.h5]
  -p <path>, --path=<path>    sub-path to engine module
                              [default: /xGadgetron/pGadgetron]
  -g, --gfactors              compute Gfactors
  -o <file>, --output=<file>  images output file
'''

__version__ = '0.1.0'
from docopt import docopt
args = docopt(__doc__, version=__version__)

import os
import sys
import time

sys.path.append(os.environ.get('SRC_PATH') + args['--path'])

from pGadgetron import *

def main():

    # acquisitions will be read from an HDF file
    input_data = AcquisitionData(args['--file'])

    # pre-process acquisitions
    print('---\n pre-processing acquisitions...')
    preprocessed_data = input_data.process(['NoiseAdjustGadget', \
         'AsymmetricEchoAdjustROGadget', 'RemoveROOversamplingGadget'])

    # set up reconstruction chain
    recon = ImagesReconstructor([\
         'AcquisitionAccumulateTriggerGadget', 'BucketToBufferGadget' \
         '(N_dimension=contrast,S_dimension=average,split_slices=false)', \
         'GenericReconCartesianReferencePrepGadget', \
         'GRAPPA:GenericReconCartesianGrappaGadget', \
         'GenericReconFieldOfViewAdjustmentGadget', \
         'GenericReconImageArrayScalingGadget', 'ImageArraySplitGadget'])
    # change a property of the gadget labelled by 'GRAPPA'
    recon.set_gadget_property('GRAPPA', 'send_out_gfactor', args['--gfactors'])
    recon.set_input(preprocessed_data)
    # reconstruct
    print('---\n reconstructing...')
    recon.process()
    output = recon.get_output()

    # show images
    output.show()

    if args['--output'] is not None:
        # write images to a new group in args.output
        # named after the current date and time
        time_str = time.asctime()
        print('writing to %s' % args['--output'])
        output.write(args['--output'], time_str)

try:
    main()
    print('done')

except error as err:
    # display error information
    print('??? %s' % err.value)
