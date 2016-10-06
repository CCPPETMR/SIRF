% Lower-level interface demo, creates and runs a chain of gadgets.

if ~libisloaded('mutilities')
    fprintf('loading mutilities library...\n')
    [notfound, warnings] = loadlibrary('mutilities');
end
if ~libisloaded('mgadgetron')
    fprintf('loading mgadgetron library...\n')
    [notfound, warnings] = loadlibrary('mgadgetron');
end

%libfunctions('mutilities')
%libfunctions('mgadgetron')

try
    % define gadgets
    gadget1 = gadgetron.Gadget('RemoveROOversamplingGadget');
	gadget2 = gadgetron.Gadget('AcquisitionAccumulateTriggerGadget');
	gadget3 = gadgetron.Gadget('BucketToBufferGadget');
	gadget4 = gadgetron.Gadget('SimpleReconGadget');
	gadget5 = gadgetron.Gadget('ImageArraySplitGadget');
	gadget6 = gadgetron.Gadget('ExtractGadget');
    
    % set gadget parameters
    gadget2.set_property('trigger_dimension', 'repetition')
    gadget3.set_property('split_slices', 'true')
    
    % create reconstructor
    recon = gadgetron.ImagesReconstructor();

    % build gadget chain
    recon.add_gadget('g1', gadget1);
	recon.add_gadget('g2', gadget2);
	recon.add_gadget('g3', gadget3);
	recon.add_gadget('g4', gadget4);
	recon.add_gadget('g5', gadget5);
	recon.add_gadget('g6', gadget6);
    
    % define raw data source
    input_data = gadgetron.MR_Acquisitions('testdata.h5');    
    recon.set_input(input_data)
    % perform reconstruction
    recon.process()
    % get reconstructed images
    images = recon.get_output();
    
    % plot reconstructed images
    for i = 1 : images.number()
        data = images.image_as_array(i);
        figure(i)
        data = data/max(max(max(data)));
        imshow(data(:,:,1));
    end

    % write images to a new group in 'output1.h5'
    % named after the current date and time
    fprintf('appending output1.h5...\n')
    images.write('output1.h5', datestr(datetime))

catch err
    % display error information
    fprintf('%s\n', err.message)
    fprintf('error id is %s\n', err.identifier)
end
