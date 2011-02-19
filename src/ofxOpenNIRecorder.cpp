#include "ofxOpenNIRecorder.h"

// undef this if you're using a PrimeSense - neccessary to not throw error on framesync (see below)
#define USINGKINECT

// Init
//----------------------------------------
ofxOpenNIRecorder::ofxOpenNIRecorder() {
	is_recording = false;
}


// Shutdown
//----------------------------------------
ofxOpenNIRecorder::~ofxOpenNIRecorder() {
	stopRecord(); // just to be sure.
}


// Setup the recorder.
//----------------------------------------
void ofxOpenNIRecorder::setup(
	 ofxOpenNIContext* pContext
	,ofxDepthGenerator* pDepth
	,ofxImageGenerator* pImage
)
{
	depth = pDepth;
	image = pImage;
	context = pContext;
}


// Start recording (once per recorder)
//----------------------------------------
bool ofxOpenNIRecorder::startRecord(string sName) {
	if(is_recording) {
		return false;
	}
	
	XnStatus result;
	
	string offname = ofToDataPath(sName, false);
	
	// recorder init
	result = recorder.Create(context->getXnContext());
	CHECK_RC(result, "Recorder create");
	
	result = recorder.SetDestination(XN_RECORD_MEDIUM_FILE, offname.c_str());
	CHECK_RC(result, "Recorder set destination");
	
	result = recorder.AddNodeToRecording(depth->getXnDepthGenerator(), XN_CODEC_16Z_EMB_TABLES);
	CHECK_RC(result, "Recorder add depth node");
	
	result = recorder.AddNodeToRecording(image->getXnImageGenerator(), XN_CODEC_NULL); // XN_CODEC_NULL appears to give least frame drops and size not much > JPEG
	CHECK_RC(result, "Recorder add image node");
		
	// Frame Sync
	xn::DepthGenerator& xn_depth = depth->getXnDepthGenerator();
	xn::ImageGenerator& xn_image = image->getXnImageGenerator();

	// Frame sync is currently not possible with Kinect cameras!!
#ifndef USINGKINECT
	if(xn_depth.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC)) {
		if(xn_depth.GetFrameSyncCap().CanFrameSyncWith(xn_image)) {
			result = xn_depth.GetFrameSyncCap().FrameSyncWith(xn_image);
			CHECK_RC(result, "Enable frame sync");
		}
	}
#endif
	
	if(xn_depth.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC)) {
		if(xn_depth.GetFrameSyncCap().CanFrameSyncWith(xn_image)) {
			result = xn_depth.GetFrameSyncCap().FrameSyncWith(xn_image);
			CHECK_RC(result, "Enable frame sync");
		}
	}
	is_recording = true;
	return true;
}

// Return recording status
//----------------------------------------
bool ofxOpenNIRecorder::isRecording() {
	return is_recording;
}
// Stop the record
//----------------------------------------
bool ofxOpenNIRecorder::stopRecord() {
	if(is_recording){
		printf("Stopped Recording");
		recorder.Unref();
		is_recording = false;
	}
	return true;
}
