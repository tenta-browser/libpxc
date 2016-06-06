/*
Copyright (c) 2013-2014, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include <string.h>
#include "pxccapturemanager.h"
#include "pxcsession.h"

/* Forward declaration of known modules */
class PXCFaceModule;
class PXCHandModule;
class PXCHandCursorModule;
class PXCBlobModule;
class PXCPersonTrackingModule;
class PXCTouchlessController;
class PXC3DSeg;
class PXC3DScan;
class PXCScenePerception;
class PXCEnhancedVideo;
class PXCObjectRecognitionModule;
class PXCTracker;
/* and their interface identifiers */
enum {
    PXC_CUID_FACE_MODULE            = PXC_UID('F', 'A', '3', 'D'),
    PXC_CUID_HAND_MODULE            = PXC_UID('H', 'A', 'N', 'N'),
    PXC_CUID_HAND_CURSOR_MODULE     = PXC_UID('H', 'C', 'M', 'N'),
    PXC_CUID_BLOB_MODULE            = PXC_UID('B', 'M', 'M', 'D'),
    PXC_CUID_PERSON_TRACKING_MODULE = PXC_UID('P', 'O', 'T', 'M'),
    PXC_CUID_TOUCHLESS_CONTROLLER   = PXC_UID('F', 'L', 'K', 'S'),
    PXC_CUID_3D_SEG                 = PXC_UID('S', 'G', 'I', '1'),
    PXC_CUID_3D_SCAN                = PXC_UID('S', 'C', 'I', '1'),
    PXC_CUID_SCENE_PERCEPTION       = PXC_UID('S', 'C', 'N', 'P'),
    PXC_CUID_ENHANCED_VIDEO         = PXC_UID('E', 'V', 'I', 'N'),
    PXC_CUID_OBJECT_RECOGNITION_MODULE = PXC_UID('O', 'B', 'J', 'M'),
    PXC_CUID_TRACKER                = PXC_UID('T', 'R', 'K', 'R'),
};

/**
    This is the main interface for the SDK pipeline.
    Control the pipeline execution with this interface.
*/
class PXCSenseManager: public PXCBase {
public:

    PXC_CUID_OVERWRITE(0xD8954321);
    PXC_DEFINE_CONST(TIMEOUT_INFINITE,-1);

    /**
        The PXCSenseManager callback instance.
    */
    class Handler {
    public:

        /**
            @brief The SenseManager calls back this function when there is a device connection or
            disconnection. During initialization, the SenseManager callbacks this function when 
            openning or closing any capture devices.
            @param[in] device           The video device instance.
            @param[in] connected        The device connection status.
            @return The return status is ignored during the PXCSenseManager initialization. During
            streaming, the SenseManager aborts the execution pipeline if the status is an error.
        */
        virtual pxcStatus PXCAPI OnConnect(PXCCapture::Device* /*device*/, pxcBool /*connected*/) { 
            return PXC_STATUS_DEVICE_FAILED; 
        }

        /**
            @brief The SenseManager calls back this function during initialization after each device 
            configuration is set.
            @param[in] mid          The module identifier. Usually this is the interface identifier, or PXCCapture::CUID+n for raw video streams. 
            @param[in] module       The module instance, or NULL for raw video streams.
            @return The SenseManager aborts the execution pipeline if the status is an error.
        */
        virtual pxcStatus PXCAPI OnModuleSetProfile(pxcUID /*mid*/, PXCBase* /*module*/) { 
            return PXC_STATUS_NO_ERROR; 
        }

        /**
            @brief The SenseManager calls back this function after a module completed processing the frame data.
            @param[in] mid          The module identifier. Usually this is the interface identifier. 
            @param[in] module       The module instance.
            @return The SenseManager aborts the execution pipeline if the status is an error.
        */
        virtual pxcStatus PXCAPI OnModuleProcessedFrame(pxcUID /*mid*/, PXCBase* /*module*/, PXCCapture::Sample* /*sample*/) { 
            return PXC_STATUS_NO_ERROR; 
        }

        /**
            @brief The SenseManager calls back this function when raw video streams (explicitly requested) are available.
            @param[in] mid          The module identifier. Usually this is the interface identifier. 
            @param[in] sample       The sample from capture device
            @return The SenseManager aborts the execution pipeline if the status is an error.
        */
        virtual pxcStatus PXCAPI OnNewSample(pxcUID /*mid*/, PXCCapture::Sample* /*sample*/) {
            return PXC_STATUS_NO_ERROR; 
        }

        /**
            @brief The SenseManager calls back this function when error or warning detected during streaming
            @param[in] mid          The identifier of module reported error or warning
            @param[in] sts          The error or warning code
        */
        virtual void PXCAPI OnStatus(pxcUID /*mid*/, pxcStatus /*sts*/) {
        }
    };

    /**
        @brief    Return the PXCSession instance. Internally managed. Do not release the instance.
        The session instance is managed internally by the SenseManager. Do not release the session instance.
        @return The PXCSession instance.
    */
    virtual PXCSession* PXCAPI QuerySession(void)=0;

    /**
        @brief    Return the PXCCaptureManager instance. Internally managed. Do not release the instance.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The PXCCaptureManager instance.
    */
    virtual PXCCaptureManager* PXCAPI QueryCaptureManager(void)=0;

    /**
        @brief    Return the captured sample for the specified module or explicitly/impl requested streams. 
        For modules, use mid=module interface identifier. 
        For explictly requested streams via multiple calls to EnableStream(s), use mid=PXCCapture::CUID+0,1,2... 
        The captured sample is managed internally by the SenseManager. Do not release the instance.
        @param[in] mid        The module identifier. Usually this is the interface identifier, or PXCCapture::CUID+n for raw video streams.
        @return The sample instance, or NULL if the captured sample is not available.
    */
    virtual PXCCapture::Sample* PXCAPI QuerySample(pxcUID mid)=0;

    /**
    @brief    Return available captured sample, explicitly or implicitly requested.
    The captured sample is managed internally by the SenseManager. Do not release the sample.
    @return The sample instance, or NULL if the captured sample is not available.
    */
    __inline PXCCapture::Sample* QuerySample(void) {
        return QuerySample(0);
    }

    /**
        @brief    Return the captured sample for the face module.
        The captured sample is managed internally by the SenseManager. Do not release the sample.
        @return The sample instance, or NULL if the captured sample is not available.
    */
    __inline PXCCapture::Sample* QueryFaceSample(void) {
        return QuerySample(PXC_CUID_FACE_MODULE);
    }

    /**
        @brief    Return the captured sample for the hand module.
        The captured sample is managed internally by the SenseManager. Do not release the sample.
        @return The sample instance, or NULL if the captured sample is not available.
    */
    __inline PXCCapture::Sample* QueryHandSample(void) {
        return QuerySample(PXC_CUID_HAND_MODULE);
    }


	/**
        @brief    Return the captured sample for the hand cursor module.
        The captured sample is managed internally by the SenseManager. Do not release the sample.
        @return The sample instance, or NULL if the captured sample is not available.
    */
    __inline PXCCapture::Sample* QueryHandCursorSample(void) {
        return QuerySample(PXC_CUID_HAND_CURSOR_MODULE);
    }

	/**
        @brief    Return the captured sample for the object recognition module.
        The captured sample is managed internally by the SenseManager. Do not release the sample.
        @return The sample instance, or NULL if the captured sample is not available.
    */
    __inline PXCCapture::Sample* QueryObjectRecognitionSample(void) {
		return QuerySample(PXC_CUID_OBJECT_RECOGNITION_MODULE);
    }

    /**
        @brief    Return the captured sample for the blob module.
        The captured sample is managed internally by the SenseManager. Do not release the sample.
        @return The sample instance, or NULL if the captured sample is not available.
    */
    __inline PXCCapture::Sample* QueryBlobSample(void) {
        return QuerySample(PXC_CUID_BLOB_MODULE);
    }

	/**
        @brief    Return the captured sample for the Person tracking module.
        The captured sample is managed internally by the SenseManager. Do not release the sample.
        @return The sample instance, or NULL if the captured sample is not available.
    */
	__inline PXCCapture::Sample* QueryPersonTrackingSample(void) {
		return QuerySample(PXC_CUID_PERSON_TRACKING_MODULE);
    }

    /**
        @brief    Return the captured sample for the scene perception module.
        The captured sample is managed internally by the SenseManager. Do not release the sample.
        @return The sample instance, or NULL if the captured sample is not available.
    */
    __inline PXCCapture::Sample* QueryScenePerceptionSample(void) {
        return QuerySample(PXC_CUID_SCENE_PERCEPTION);
    }

    /**
		@brief	  Return the captured sample for the object tracker module.
		The captured sample is managed internally by the SenseManager. Do not release the sample.
		@return The sample instance, or NULL if the captured sample is not available.
	*/
	__inline const PXCCapture::Sample* QueryTrackerSample(void) {
		return QuerySample(PXC_CUID_TRACKER);
	}

	/**
		@brief	  Return the captured sample for the enhanced Videography module.
		The captured sample is managed internally by the SenseManager. Do not release the sample.
		@return The sample instance, or NULL if the captured sample is not available.
	*/
	__inline const PXCCapture::Sample* QueryEnhancedVideoSample(void) {
		return QuerySample(PXC_CUID_ENHANCED_VIDEO);
	}

    /**
        @brief    Return the module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @param[in] mid        The module identifier. Usually this is the interface identifier.
        @return The module instance.
    */
    virtual PXCBase* PXCAPI QueryModule(pxcUID mid)=0;

    /**
        @brief    Return the Face module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
    __inline PXCFaceModule* QueryFace(void) { 
        PXCBase *instance=QueryModule(PXC_CUID_FACE_MODULE);
        return instance ? (PXCFaceModule*)instance->QueryInstance(PXC_CUID_FACE_MODULE):0;
    }

	__inline PXCPersonTrackingModule* QueryPersonTracking(void) {
		PXCBase *instance = QueryModule(PXC_CUID_PERSON_TRACKING_MODULE);
		return instance ? (PXCPersonTrackingModule*)instance->QueryInstance(PXC_CUID_PERSON_TRACKING_MODULE) : 0;
    }

    /**
		@brief	Return the tracker module instance. Between AcquireFrame/ReleaseFrame, the function returns
		NULL if the specified module hasn't completed processing the current frame of image data.
		The instance is managed internally by the SenseManager. Do not release the instance.
		@return The module instance.
	*/
	__inline PXCTracker* QueryTracker(void) { 
		PXCBase *instance=QueryModule(PXC_CUID_TRACKER);
		return instance ? (PXCTracker*)instance->QueryInstance(PXC_CUID_TRACKER):0;
	}


    /**
        @brief    Return the hand module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
    __inline PXCHandModule* QueryHand(void) { 
        PXCBase *instance=QueryModule(PXC_CUID_HAND_MODULE);
        return instance ? (PXCHandModule*)instance->QueryInstance(PXC_CUID_HAND_MODULE):0;
    }

	 /**
        @brief    Return the hand module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
    __inline PXCHandCursorModule* QueryHandCursor(void) { 
        PXCBase *instance=QueryModule(PXC_CUID_HAND_CURSOR_MODULE);
        return instance ? (PXCHandCursorModule*)instance->QueryInstance(PXC_CUID_HAND_CURSOR_MODULE):0;
    }

	/**
        @brief    Return the object recognition module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
	__inline PXCObjectRecognitionModule* QueryObjectRecognition(void) { 
		PXCBase *instance=QueryModule(PXC_CUID_OBJECT_RECOGNITION_MODULE);
		return instance ? (PXCObjectRecognitionModule*)instance->QueryInstance(PXC_CUID_OBJECT_RECOGNITION_MODULE):0;
    }

    /**
        @brief    Return the blob module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
    __inline PXCBlobModule* QueryBlob(void) { 
        PXCBase *instance=QueryModule(PXC_CUID_BLOB_MODULE);
        return instance ? (PXCBlobModule*)instance->QueryInstance(PXC_CUID_BLOB_MODULE):0;
    }


	/**
        @brief    Return the Person tracking module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
	__inline PXCPersonTrackingModule* QueryPersonTacking(void) {
		PXCBase *instance = QueryModule(PXC_CUID_PERSON_TRACKING_MODULE);
		return instance ? (PXCPersonTrackingModule*)instance->QueryInstance(PXC_CUID_PERSON_TRACKING_MODULE) : 0;
    }


    /**
        @brief    Return the Touchless module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
    __inline PXCTouchlessController* QueryTouchlessController(void) { 
        PXCBase *instance=QueryModule(PXC_CUID_TOUCHLESS_CONTROLLER);
        return instance ? (PXCTouchlessController*)instance->QueryInstance(PXC_CUID_TOUCHLESS_CONTROLLER):0;
    }

    /**
        @brief    Return the 3D Segmentation module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
    __inline PXC3DSeg* Query3DSeg(void) { 
        PXCBase *instance=QueryModule(PXC_CUID_3D_SEG);
        return instance ? (PXC3DSeg*)instance->QueryInstance(PXC_CUID_3D_SEG):0;
    }
    
    /**
        @brief    Return the Mesh Capture module instance. Between AcquireFrame/ReleaseFrame, the function returns
        NULL if the specified module hasn't completed processing the current frame of image data.
        The instance is managed internally by the SenseManager. Do not release the instance.
        @return The module instance.
    */
    __inline PXC3DScan* Query3DScan(void) { 
        PXCBase *instance=QueryModule(PXC_CUID_3D_SCAN);
        return instance ? (PXC3DScan*)instance->QueryInstance(PXC_CUID_3D_SCAN):0;
    }

    /**
    @brief    Return the Scene Perception module instance. Between AcquireFrame/ReleaseFrame, the function returns
    NULL if the specified module hasn't completed processing the current frame of image data.
    The instance is managed internally by the SenseManager. Do not release the instance.
    @return The module instance.
    */
    __inline PXCScenePerception* QueryScenePerception(void) {
        PXCBase *instance = QueryModule(PXC_CUID_SCENE_PERCEPTION);
        return instance ? (PXCScenePerception*)instance->QueryInstance(PXC_CUID_SCENE_PERCEPTION) : 0;
    }

	 /**
		@brief	Return the Enhanced Videography module instance. Between AcquireFrame/ReleaseFrame, the function returns
		NULL if the specified module hasn't completed processing the current frame of image data.
		The instance is managed internally by the SenseManager. Do not release the instance.
		@return The module instance.
	*/
	__inline PXCEnhancedVideo* QueryEnhancedVideo(void) { 
		PXCBase *instance=QueryModule(PXC_CUID_ENHANCED_VIDEO);
		return instance ? (PXCEnhancedVideo*)instance->QueryInstance(PXC_CUID_ENHANCED_VIDEO):0;
	}

    /**
        @brief    Initialize the SenseManager pipeline for streaming with callbacks. The application must 
        enable raw streams or algorithm modules before this function.
        @param[in] handler          Optional callback instance. 
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    virtual pxcStatus PXCAPI Init(Handler *handler)=0;

    /**
        @brief    Initialize the SenseManager pipeline for streaming. The application must enable raw 
        streams or algorithm modules before this function.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus Init(void) { 
        return Init(0); 
    }

    /**
        @brief    Stream frames from the capture module to the algorithm modules. The application must 
        initialize the pipeline before calling this function. If blocking, the function blocks until
        the streaming stops (upon any capture device error or any callback function returns any error.
        If non-blocking, the function returns immediately while running streaming in a thread.
        AcquireFrame/ReleaseFrame are not compatible with StreamFrames. Run the SenseManager in the pulling
        mode with AcquireFrame/ReleaseFrame, or the callback mode with StreamFrames.
        @param[in]    blocking      The blocking status.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    virtual pxcStatus PXCAPI StreamFrames(pxcBool blocking)=0;

    /**
        @brief    This function returns the input device connection status during streaming.
        The connection status is valid only in between the Init function and the Close function.
        @return true        The input device is connected.
        @return false       The input device is not connected.
    */
    virtual pxcBool PXCAPI IsConnected(void)=0;

    /**
        @brief    This function starts streaming and waits until certain events occur. If ifall=true, 
        the function blocks until all samples are ready and the modules completed processing the samples.
        If ifall=false, the function blocks until any of the mentioned is ready. The SenseManager 
        pipeline pauses at this point for the application to retrieve the processed module data, until 
        the application calls ReleaseFrame.
        AcquireFrame/ReleaseFrame are not compatible with StreamFrames. Run the SenseManager in the pulling
        mode with AcquireFrame/ReleaseFrame, or the callback mode with StreamFrames.
        @param[in]    ifall             If true, wait for all modules to complete processing the data.
        @param[in]    timeout           The time out value in milliseconds.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    virtual pxcStatus PXCAPI AcquireFrame(pxcBool ifall, pxcI32 timeout)=0;

    /**
        @brief    This function starts streaming and waits until certain events occur. If ifall=true, 
        the function blocks until all samples are ready and the modules completed processing the samples.
        If ifall=false, the function blocks until any of the mentioned is ready. The SenseManager 
        pipeline pauses at this point for the application to retrieve the processed module data, until 
        the application calls ReleaseFrame.
        AcquireFrame/ReleaseFrame are not compatible with StreamFrames. Run the SenseManager in the pulling
        mode with AcquireFrame/ReleaseFrame, or the callback mode with StreamFrames.
        @param[in]    ifall         If true, wait for all modules to complete processing the data.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus AcquireFrame(pxcBool ifall=true) { 
        return AcquireFrame(ifall, (pxcI32)TIMEOUT_INFINITE); 
    }

    /**
        @reserved DO NOT USE THIS FUNCTION.
        @brief    This function discards any internally cached sample from the capture device (and restart
        new streaming.) Use this function together after file playback repositioning to avoid any caching 
        effects. Use also this function in the snapshot mode to get the most recent sample without any 
        streaming caching effect.
    */
    virtual void PXCAPI FlushFrame(void)=0;

    /**
        @brief    This function resumes streaming after AcquireFrame.
        AcquireFrame/ReleaseFrame are not compatible with StreamFrames. Run the SenseManager in the pulling
        mode with AcquireFrame/ReleaseFrame, or the callback mode with StreamFrames.
    */
    virtual void PXCAPI ReleaseFrame(void)=0;

    /**
        @brief    This function closes the execution pipeline.
    */
    virtual void PXCAPI Close(void)=0;

    /**
        @brief    Explicitly request to stream the specified raw streams. If specified more than a stream, 
        SenseManager will synchronize these streams. If called multiple times, the function treats each
        stream request as independent (unaligned.) The stream identifier is PXCCapture::CUID+n.
        @param[in] sdesc            The stream descriptor.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    virtual pxcStatus PXCAPI EnableStreams(PXCVideoModule::DataDesc *sdesc)=0;

    /**
        @brief    Explicitly request to stream the specified raw stream. If specified more than one stream, 
        SenseManager will synchronize these streams. If called multiple times, the function treats each
        stream request as independent (unaligned). The stream identifier is PXCCapture::CUID+n.
        @param[in] type             The stream type.
        @param[in] width            Optional width.
        @param[in] height           Optional height.
        @param[in] fps              Optional frame rate.
		@param[in] option			Optional stream flags.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus EnableStream(PXCCapture::StreamType type, pxcI32 width=0, pxcI32 height=0, pxcF32 fps=0, PXCCapture::Device::StreamOption options=PXCCapture::Device::STREAM_OPTION_ANY) {
        PXCVideoModule::DataDesc ddesc={};
        ddesc.deviceInfo.streams = type;
        PXCVideoModule::StreamDesc& sdesc=ddesc.streams[type];
        sdesc.sizeMin.width=sdesc.sizeMax.width=width;
        sdesc.sizeMin.height=sdesc.sizeMax.height=height;
        sdesc.frameRate.min=fps;
        sdesc.frameRate.max=fps;
		sdesc.options = options;
        return EnableStreams(&ddesc);
    }

    /**
        @brief    Enable a module in the pipeline.
        @param[in] mid              The module identifier. This is usually the interface identifier.
        @param[in] mdesc            The module descriptor.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    virtual pxcStatus PXCAPI EnableModule(pxcUID mid, PXCSession::ImplDesc *mdesc)=0;

    /**
        @brief    Enable the face module in the pipeline.
        @param[in] name             The optional module name.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus EnableFace(pxcCHAR *name=0) {
        PXCSession::ImplDesc mdesc;
        memset(&mdesc,0,sizeof(mdesc));
        mdesc.cuids[0]=PXC_CUID_FACE_MODULE;
        if (name) PXC_STRCPY(mdesc.friendlyName, name, sizeof(mdesc.friendlyName)/sizeof(pxcCHAR));
        return EnableModule(PXC_CUID_FACE_MODULE,&mdesc);
    }

    /**
		@brief	Enable the Tracker module in the pipeline.
		@return PXC_STATUS_NO_ERROR		Successful execution.
	*/
	__inline pxcStatus EnableTracker(void) {
		PXCSession::ImplDesc mdesc;
		memset(&mdesc,0,sizeof(mdesc));
		mdesc.cuids[0]=PXC_CUID_TRACKER;
		return EnableModule(PXC_CUID_TRACKER,&mdesc);
	}


    /**
        @brief    Enable the hand module in the pipeline.
        @param[in] name        The optional module name.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus EnableHand(pxcCHAR *name=0) {
        PXCSession::ImplDesc mdesc;
        memset(&mdesc,0,sizeof(mdesc));
        mdesc.cuids[0]=PXC_CUID_HAND_MODULE;
        if (name) PXC_STRCPY(mdesc.friendlyName, name, sizeof(mdesc.friendlyName)/sizeof(pxcCHAR));
        return EnableModule(PXC_CUID_HAND_MODULE,&mdesc);
    }


	 /**
        @brief    Enable the hand cursor module in the pipeline.
        @param[in] name        The optional module name.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus EnableHandCursor(pxcCHAR *name=0) {
        PXCSession::ImplDesc mdesc;
        memset(&mdesc,0,sizeof(mdesc));
        mdesc.cuids[0]=PXC_CUID_HAND_CURSOR_MODULE;
        if (name) PXC_STRCPY(mdesc.friendlyName, name, sizeof(mdesc.friendlyName)/sizeof(pxcCHAR));
        return EnableModule(PXC_CUID_HAND_CURSOR_MODULE,&mdesc);
    }

    /**
        @brief    Enable the blob module in the pipeline.
        @param[in] name        The optional module name.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus EnableBlob(pxcCHAR *name=0) {
        PXCSession::ImplDesc mdesc;
        memset(&mdesc,0,sizeof(mdesc));
        mdesc.cuids[0]=PXC_CUID_BLOB_MODULE;
        if (name) PXC_STRCPY(mdesc.friendlyName, name, sizeof(mdesc.friendlyName)/sizeof(pxcCHAR));
        return EnableModule(PXC_CUID_BLOB_MODULE,&mdesc);
    }


	/**
        @brief    Enable the Person tracking module in the pipeline.
        @param[in] name        The optional module name.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
	__inline pxcStatus EnablePersonTracking(pxcCHAR *name = 0) {
        PXCSession::ImplDesc mdesc;
        memset(&mdesc,0,sizeof(mdesc));
		mdesc.cuids[0] = PXC_CUID_PERSON_TRACKING_MODULE;
        if (name) PXC_STRCPY(mdesc.friendlyName, name, sizeof(mdesc.friendlyName)/sizeof(pxcCHAR));
		return EnableModule(PXC_CUID_PERSON_TRACKING_MODULE, &mdesc);
    }


    /**
        @brief    Enable the touchless controller module in the pipeline.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus EnableTouchlessController(void){
        PXCSession::ImplDesc mdesc;
        memset(&mdesc,0,sizeof(mdesc));
        mdesc.cuids[0]=PXC_CUID_TOUCHLESS_CONTROLLER;
        return EnableModule(PXC_CUID_TOUCHLESS_CONTROLLER,&mdesc);
    }

    /**
        @brief    Enable the 3D Segmentation module in the pipeline.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus Enable3DSeg(pxcCHAR *name = NULL) {
        PXCSession::ImplDesc mdesc;
        memset(&mdesc,0,sizeof(mdesc));
        mdesc.cuids[0]=PXC_CUID_3D_SEG;
        if (name) PXC_STRCPY(mdesc.friendlyName, name, sizeof(mdesc.friendlyName)/sizeof(pxcCHAR));
        return EnableModule(PXC_CUID_3D_SEG,&mdesc);
    }
    

    /**
        @brief    Enable the Mesh Capture module in the pipeline.
        @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus Enable3DScan(pxcCHAR *name = NULL) {
        PXCSession::ImplDesc mdesc;
        memset(&mdesc,0,sizeof(mdesc));
        mdesc.cuids[0]=PXC_CUID_3D_SCAN;
        if (name) PXC_STRCPY(mdesc.friendlyName, name, sizeof(mdesc.friendlyName)/sizeof(pxcCHAR));
        return EnableModule(PXC_CUID_3D_SCAN,&mdesc);
    }

    /**
    @brief    Enable the Scene Perception module in the pipeline.
    @return PXC_STATUS_NO_ERROR        Successful execution.
    */
    __inline pxcStatus EnableScenePerception(pxcCHAR *name = NULL) {
        PXCSession::ImplDesc mdesc;
        memset(&mdesc, 0, sizeof(mdesc));
        mdesc.cuids[0] = PXC_CUID_SCENE_PERCEPTION;
        if (name) PXC_STRCPY(mdesc.friendlyName, name, sizeof(mdesc.friendlyName)/sizeof(pxcCHAR));
        return EnableModule(PXC_CUID_SCENE_PERCEPTION, &mdesc);
    }

	/**
		@brief	Enable the Enhanced Videogrphy module in the pipeline.
		@return PXC_STATUS_NO_ERROR		Successful execution.
	*/
	__inline pxcStatus EnableEnhancedVideo(void) {
		PXCSession::ImplDesc mdesc;
		memset(&mdesc,0,sizeof(mdesc));
		mdesc.cuids[0]=PXC_CUID_ENHANCED_VIDEO;
		return EnableModule(PXC_CUID_ENHANCED_VIDEO,&mdesc);
	}

	/**
		@brief	Enable the Object recognition module in the pipeline.
		@return PXC_STATUS_NO_ERROR		Successful execution.
	*/
	__inline pxcStatus EnableObjectRecognition(void) {
		PXCSession::ImplDesc mdesc;
		memset(&mdesc,0,sizeof(mdesc));
		mdesc.cuids[0]=PXC_CUID_OBJECT_RECOGNITION_MODULE;
		return EnableModule(PXC_CUID_OBJECT_RECOGNITION_MODULE,&mdesc);
	}
	
    /**
        @brief    Pause/Resume the execution of the specified module.
        @param[in] mid          The module identifier. This is usually the interface identifier.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
    virtual void PXCAPI PauseModule(pxcUID mid, pxcBool pause)=0;

	/**
        @brief    Pause/Resume the execution of the Scene Perception module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
	__inline void PauseScenePerception(pxcBool pause) { 
		PauseModule(PXC_CUID_SCENE_PERCEPTION,pause); 
	}

		/**
        @brief    Pause/Resume the execution of the Object Recognition module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
	__inline void PauseObjectRecognition(pxcBool pause) { 
		PauseModule(PXC_CUID_OBJECT_RECOGNITION_MODULE,pause); 
	}


    /**
        @brief    Pause/Resume the execution of the face module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
    __inline void PauseFace(pxcBool pause) { 
        PauseModule(PXC_CUID_FACE_MODULE,pause); 
    }

    /**
		@brief	Pause/Resume the execution of the Tracker module.
		@param[in] pause	If true, pause the module. Otherwise, resume the module.
	*/
    __inline void PauseTracker(pxcBool pause) {
		PauseModule(PXC_CUID_TRACKER,pause); 
	}

    /**
        @brief    Pause/Resume the execution of the hand module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
    __inline void PauseHand(pxcBool pause) {
        PauseModule(PXC_CUID_HAND_MODULE,pause); 
    }

	 /**
        @brief    Pause/Resume the execution of the hand cursor module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
    __inline void PauseHandCursor(pxcBool pause) {
        PauseModule(PXC_CUID_HAND_CURSOR_MODULE,pause); 
    }

    /**
        @brief    Pause/Resume the execution of the blob module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
    __inline void PauseBlob(pxcBool pause) {
        PauseModule(PXC_CUID_BLOB_MODULE,pause); 
    }


	/**
        @brief    Pause/Resume the execution of the Person tracking module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
	__inline void PausePersonTracking(pxcBool pause) {
		PauseModule(PXC_CUID_PERSON_TRACKING_MODULE, pause);
    }

    /**
        @brief    Pause/Resume the execution of the touchless controller module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
    __inline void PauseTouchlessController(pxcBool pause) {
        PauseModule(PXC_CUID_TOUCHLESS_CONTROLLER,pause); 
    }

    /**
        @brief    Pause/Resume the execution of the 3D Segmentation module.
        @param[in] pause        If true, pause the module. Otherwise, resume the module.
    */
    __inline void Pause3DSeg(pxcBool pause) {
        PauseModule(PXC_CUID_3D_SEG,pause);
    }

	/**
		@brief	Pause/Resume the execution of the Enhanced Videography module.
		@param[in] pause	If true, pause the module. Otherwise, resume the module.
	*/
    __inline void PauseEnhancedVideo(pxcBool pause) {
		PauseModule(PXC_CUID_ENHANCED_VIDEO,pause); 
	}

    /**
        @brief    Create an instance of the PXCSenseManager interface.
        @return The PXCSenseManager instance.
    */
    __inline static PXCSenseManager* CreateInstance(void) {
        PXCSession *session=PXCSession_Create();
        if (!session) return 0;
        PXCSenseManager *sm=session->CreateSenseManager();
        session->Release();
        return sm;
    }
};
