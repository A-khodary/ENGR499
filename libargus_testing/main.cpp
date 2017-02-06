// This code is taken from the "Get Started with the JetPack Camera API" video at 48:11

//#include "Error.h"
//#include "Thread.h"

#include <Argus/Argus.h>
#include <EGLStream/EGLStream.h>
#include <EGLStream/NV/ImageNativeBuffer.h>

#include <NVEglRenderer.h>
#include <NvJpegEncoder.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <fstream>

int main() {
  std::cout << "Starting image capture test" << std::endl;

  // Create output stream.
  UniqueObj<OutputStream> stream(iSession->createOutputStream(streamSettings));

  // Create and connect FrameConsumer to output stream.
  UniqueObj<EGLStream::FrameConsumer> consumer(EGLStream::FrameConsumer::create(stream));
  EGLStraem::IFrameConsumer *iConsumer = interface_cast<EGLStream::IFrameConsumer>(consumer.get());

  // Submit capture request outputting to stream.
  iSession->capture(request);

  // Acquire a Frame from the consumer.
  UniqueObj<EGLStream::Frame> frame(iConsumer->acquireFrame());
  EGLStream::IFrame *iFrame = interface_cast<EGLStream::IFrame>(frame);

  // Get the Argus::CaptureMetadata embedded in the frame.
  EGLStream::IArgusCaptureMetadata *iArgusMetadata = interface_cast<IArgusCaptureMetadata>(frame);
  Argus::CaptureMetadata* metadata = iArgusMetadata->getMetadata();

  // Get the Image from the Frame.
  EGLStream::Image *image = iFrame->getImage();

  // Use the JPEG interface to encode and write the JPEG file.
  EGLStream::IImageJPEG *iImageJPEG = interface_cast<EGLStream::IImageJPEG>(image);
  iImageJPEG->writeJPEG("filename.jpg");

  return 0;
}
