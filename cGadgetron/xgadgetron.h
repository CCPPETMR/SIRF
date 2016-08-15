/*
CCP PETMR toolbox
Copyright 2015 - 2016 Rutherford Appleton Laboratory STFC
Copyright 2015 - 2016 Institute of Nuclear Medicine, University College London.

This product includes software developed for the Collaborative Computational
Project in Positron Emission Tomograph and Magnetic Resonance imaging
(http://www.ccppetmr.ac.uk/) at RAL STFC (http://www.stfc.ac.uk) and
the Institute of Nuclear Medicine(http ://www.ucl.ac.uk/medicine/nuclear-medicine).

See xGadgetron/LICENSE.txt for license details.

*/

/*!
\file
\ingroup Gadgetron Extensions
\brief Specification file for extended Gadgetron functionality classes.

\author Evgueni Ovtchinnikov
\author CCP PETMR
*/

#ifndef GADGETRON_EXTENSIONS
#define GADGETRON_EXTENSIONS

#include <cmath>
#include <string>

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <ismrmrd/ismrmrd.h>
#include <ismrmrd/dataset.h>
#include <ismrmrd/meta.h>
#include <ismrmrd/xml.h>

#include "gadgetron_client.h"
#include "gadget_lib.h"
#include "ismrmrd_fftw.h"
#include "localised_exception.h"

#define N_TRIALS 5

/*!
\ingroup Gadgetron Extensions
\brief Shared pointer wrap-up for GadgetronClientConnector.

*/

class GTConnector {
public:
	GTConnector() 
	{
		sptr_con_ = boost::shared_ptr<GadgetronClientConnector>
			(new GadgetronClientConnector);
	}
	GadgetronClientConnector& operator()() 
	{
		return *sptr_con_.get();
	}
	boost::shared_ptr<GadgetronClientConnector> sptr() 
	{
		return sptr_con_;
	}
private:
	boost::shared_ptr<GadgetronClientConnector> sptr_con_;
};

/*!
\ingroup Gadgetron Extensions
\brief Shared pointer wrap-up for the abstract gadget class aGadget.

*/

class GadgetHandle {
public:
	GadgetHandle(std::string id, boost::shared_ptr<aGadget> sptr_g) : 
		id_(id), sptr_g_(sptr_g) {}
	std::string id() const 
	{
		return id_;
	}
	aGadget& gadget() 
	{
		return *sptr_g_.get();
	}
	const aGadget& gadget() const 
	{
		return *sptr_g_.get();
	}
private:
	std::string id_;
	boost::shared_ptr<aGadget> sptr_g_;
};

/*!
\ingroup Gadgetron Extensions
\brief Gadget chain class.

Gadgetron gadget chains have the following form:

reader gadget 
(receives data from the client and passes it down the chain)
-
first gadget 
(processes data received from the reader gadget and passes the result on)
-
second gadget 
(processes data received from the first gadget and passes the result on)
-
...
-
last gadget
(as above)
-
finishing gadget
(prepares the final result for sending to the client)
-
writer gadget
(sends the final result to the client)
*/

class GadgetChain {
public:
	virtual ~GadgetChain() {}
	// adds reader gadget
	void add_reader(std::string id, boost::shared_ptr<aGadget> sptr_g) 
	{
			readers_.push_back(boost::shared_ptr<GadgetHandle>
				(new GadgetHandle(id, sptr_g)));
	}
	// adds writer gadget
	void add_writer(std::string id, boost::shared_ptr<aGadget> sptr_g) 
	{
		writers_.push_back(boost::shared_ptr<GadgetHandle>
			(new GadgetHandle(id, sptr_g)));
	}
	// sdds finishig gadget
	void set_endgadget(boost::shared_ptr<aGadget> sptr_g) 
	{
		endgadget_ = sptr_g;
	}
	// adds any other gadget
	void add_gadget(std::string id, boost::shared_ptr<aGadget> sptr_g)
	{
		gadgets_.push_back(boost::shared_ptr<GadgetHandle>
			(new GadgetHandle(id, sptr_g)));
	}
	// returns string containing the definition of the chain in xml format
	std::string xml() const;
private:
	std::list<boost::shared_ptr<GadgetHandle> > readers_;
	std::list<boost::shared_ptr<GadgetHandle> > writers_;
	std::list<boost::shared_ptr<GadgetHandle> > gadgets_;
	boost::shared_ptr<aGadget> endgadget_;
};

class AcquisitionsProcessor : public GadgetChain {
public:
	AcquisitionsProcessor() :
		host_("localhost"), port_("9002"),
		reader_(new IsmrmrdAcqMsgReader),
		writer_(new IsmrmrdAcqMsgWriter)
	{
		sptr_acqs_.reset();
		add_reader("reader", reader_);
		add_writer("writer", writer_);
		boost::shared_ptr<AcqFinishGadget> endgadget(new AcqFinishGadget);
		set_endgadget(endgadget);
	}
	virtual ~AcquisitionsProcessor() {}

	void process(AcquisitionsContainer& acquisitions);
	boost::shared_ptr<AcquisitionsContainer> get_output() 
	{
		return sptr_acqs_;
	}

private:
	std::string host_;
	std::string port_;
	boost::shared_ptr<IsmrmrdAcqMsgReader> reader_;
	boost::shared_ptr<IsmrmrdAcqMsgWriter> writer_;
	boost::shared_ptr<AcquisitionsContainer> sptr_acqs_;
};

class ImagesReconstructor : public GadgetChain {
public:

	ImagesReconstructor() :
		host_("localhost"), port_("9002"),
		reader_(new IsmrmrdAcqMsgReader),
		writer_(new IsmrmrdImgMsgWriter)
	{
		sptr_images_.reset();
		add_reader("reader", reader_);
		add_writer("writer", writer_);
		boost::shared_ptr<ImgFinishGadget> endgadget(new ImgFinishGadget);
		set_endgadget(endgadget);
	}

	void process(AcquisitionsContainer& acquisitions);
	boost::shared_ptr<ImagesContainer> get_output() 
	{
		return sptr_images_;
	}

private:
	std::string host_;
	std::string port_;
	boost::shared_ptr<IsmrmrdAcqMsgReader> reader_;
	boost::shared_ptr<IsmrmrdImgMsgWriter> writer_;
	boost::shared_ptr<ImagesContainer> sptr_images_;
};

class ImagesProcessor : public GadgetChain {
public:
	ImagesProcessor() :
		host_("localhost"), port_("9002"),
		reader_(new IsmrmrdImgMsgReader),
		writer_(new IsmrmrdImgMsgWriter)
	{
		add_reader("reader", reader_);
		add_writer("writer", writer_);
		boost::shared_ptr<ImgFinishGadget> endgadget(new ImgFinishGadget);
		set_endgadget(endgadget);
	}

	void process(ImagesContainer& images);
	boost::shared_ptr<ImagesContainer> get_output() 
	{
		return sptr_images_;
	}

private:
	std::string host_;
	std::string port_;
	boost::shared_ptr<IsmrmrdImgMsgReader> reader_;
	boost::shared_ptr<IsmrmrdImgMsgWriter> writer_;
	boost::shared_ptr<ImagesContainer> sptr_images_;
};

class AcquisitionModel {
public:

	AcquisitionModel(
		boost::shared_ptr<AcquisitionsContainer> sptr_ac,
		boost::shared_ptr<ImagesContainer> sptr_ic
		) : sptr_acqs_(sptr_ac), sptr_imgs_(sptr_ic)
	{
	}

	void setCSMs(boost::shared_ptr<CoilSensitivitiesContainer> sptr_csms)
	{
		sptr_csms_ = sptr_csms;
	}

	void fwd(ImageWrap& iw, CoilData& csm, AcquisitionsContainer& ac)
	{
		int type = iw.type();
		void* ptr = iw.ptr_image();
		IMAGE_PROCESSING_SWITCH(type, fwd_, ptr, csm, ac);
	}

	void bwd(ImageWrap& iw, CoilData& csm, AcquisitionsContainer& ac, 
		int& off)
	{
		int type = iw.type();
		void* ptr = iw.ptr_image();
		IMAGE_PROCESSING_SWITCH(type, bwd_, ptr, csm, ac, off);
	}

	void fwd(ImagesContainer& ic, CoilSensitivitiesContainer& cc,
		AcquisitionsContainer& ac);

	void bwd(ImagesContainer& ic, CoilSensitivitiesContainer& cc,
		AcquisitionsContainer& ac);

	boost::shared_ptr<AcquisitionsContainer> fwd(ImagesContainer& ic)
	{
		if (!sptr_csms_.get() || sptr_csms_->items() < 1)
			throw LocalisedException
			("coil sensitivity maps not found", __FILE__, __LINE__);
		boost::shared_ptr<AcquisitionsContainer> sptr_acqs =
			sptr_acqs_->new_acquisitions_container();
		fwd(ic, *sptr_csms_, *sptr_acqs);
		return sptr_acqs;
	}

	boost::shared_ptr<ImagesContainer> bwd(AcquisitionsContainer& ac)
	{
		if (!sptr_csms_.get() || sptr_csms_->items() < 1)
			throw LocalisedException
			("coil sensitivity maps not found", __FILE__, __LINE__);
		boost::shared_ptr<ImagesContainer> sptr_imgs =
			sptr_imgs_->new_images_container();
		bwd(*sptr_imgs, *sptr_csms_, ac);
		return sptr_imgs;
	}

private:
	std::string par_;
	boost::shared_ptr<AcquisitionsContainer> sptr_acqs_;
	boost::shared_ptr<ImagesContainer> sptr_imgs_;
	boost::shared_ptr<CoilSensitivitiesContainer> sptr_csms_;

	template< typename T>
	void fwd_(ISMRMRD::Image<T>* ptr_img, CoilData& csm,
		AcquisitionsContainer& ac);
	template< typename T>
	void bwd_(ISMRMRD::Image<T>* ptr_im, CoilData& csm,
		AcquisitionsContainer& ac, int& off);
};

#endif
