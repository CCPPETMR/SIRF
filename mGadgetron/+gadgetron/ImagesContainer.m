classdef ImagesContainer < gadgetron.DataContainer
    properties
        name_
    end
    methods
        function self = ImagesContainer()
            self.name_ = 'ImagesContainer';
            self.handle_ = [];
        end
        function delete(self)
            if ~isempty(self.handle_)
                calllib('mutilities', 'mDeleteObject', self.handle_)
                self.handle_ = [];
            end
        end
        function write(self, file, group)
            handle = calllib('mgadgetron', 'mGT_writeImages', ...
                self.handle_, file, group);
            gadgetron.checkExecutionStatus(self.name_, handle);
            calllib('mutilities', 'mDeleteDataHandle', handle)
        end
        function images = select(self, inc, off)
            images = gadgetron.ImagesContainer();
            if nargin < 3
                off = 0;
            end
            if nargin < 2
                inc = 1;
            end
            images.handle_ = calllib('mgadgetron', 'mGT_selectImages', ...
                self.handle_, inc, off);
            gadgetron.checkExecutionStatus(self.name_, images.handle_);
        end
        function images = process(self, list)
            ip = gadgetron.ImagesProcessor(list);
            images = ip.process(self);
        end
        function conversion_to_real(self, type)
            calllib('mgadgetron', 'mGT_setImageToRealConversion', ...
                self.handle_, type)
        end
        function images = real(self, ctype)
            if nargin < 2
                ctype = 1;
            end
            self.conversion_to_real(ctype);
            images = self.process({'ComplexToFloatGadget'});
        end
        function data = image_as_array(self, im_num)
            ptr_i = libpointer('int32Ptr', zeros(4, 1));
            calllib...
                ('mgadgetron', 'mGT_getImageDimensions', ...
                self.handle_, im_num - 1, ptr_i);
            dim = ptr_i.Value;
            n = dim(1)*dim(2)*dim(3)*dim(4);
            ptr_v = libpointer('doublePtr', zeros(n, 1));
            calllib...
                ('mgadgetron', 'mGT_getImageDataAsDoubleArray', ...
                self.handle_, im_num - 1, ptr_v)
            data = reshape(ptr_v.Value, dim(1), dim(2), dim(3), dim(4));
        end
    end
end