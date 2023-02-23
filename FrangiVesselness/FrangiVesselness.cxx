#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
// Itk's frangi vesselness from Antiga implementation
#include "itkHessianToObjectnessMeasureImageFilter.h"
// Our own
//#include "itkHessianToFrangiMeasureImageFilter.h"

#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkCastImageFilter.h"


#include "FrangiVesselnessCLP.h"

namespace {
template<class T>
int DoIt(int argc, char *argv[]) {
        PARSE_ARGS;
  
  constexpr unsigned int Dimension = 3;
  using PixelType = T;
  using InputImageType = itk::Image<T, Dimension>;
  using ImageType = itk::Image<double, Dimension>;
  using OutputImageType = itk::Image<double, Dimension>;
  using ReaderType = itk::ImageFileReader<InputImageType>;

  using MaskPixelType = uint8_t;
  using MaskImageType = itk::Image<MaskPixelType, Dimension>;
  using MaskReaderType = itk::ImageFileReader<MaskImageType>;

  auto reader = ReaderType::New();
  reader->SetFileName(inputVolume);

  using CastType = itk::CastImageFilter<InputImageType, ImageType>;
  auto cast = CastType::New();
  cast->SetInput(reader->GetOutput());

  auto image = cast->GetOutput();
  MaskImageType::Pointer maskImage;
  // Antiga vesselness operator
  
  using HessianPixelType = itk::SymmetricSecondRankTensor< double, Dimension >;
  using HessianImageType = itk::Image< HessianPixelType, Dimension >;
  using ObjectnessFilterType = itk::HessianToObjectnessMeasureImageFilter< HessianImageType, ImageType >;
  //using ObjectnessFilterType = itk::HessianToFrangiMeasureImageFilter<HessianImageType, ImageType>;//
  
  ObjectnessFilterType::Pointer objectnessFilter = ObjectnessFilterType::New();
  objectnessFilter->SetBrightObject( true );
  objectnessFilter->SetScaleObjectnessMeasure( false );
  objectnessFilter->SetAlpha( alpha );
  objectnessFilter->SetBeta( beta );
  objectnessFilter->SetGamma( gamma );
  
  using MultiScaleEnhancementFilterType = itk::MultiScaleHessianBasedMeasureImageFilter< ImageType, HessianImageType, ImageType >;
  MultiScaleEnhancementFilterType::Pointer multiScaleEnhancementFilter =  MultiScaleEnhancementFilterType::New();
  multiScaleEnhancementFilter->SetInput( image );
  multiScaleEnhancementFilter->SetHessianToMeasureFilter( objectnessFilter );
  multiScaleEnhancementFilter->SetSigmaStepMethodToLogarithmic();
  multiScaleEnhancementFilter->SetSigmaMinimum( sigmaMin );
  multiScaleEnhancementFilter->SetSigmaMaximum( sigmaMax );
  multiScaleEnhancementFilter->SetNumberOfSigmaSteps( numberOfScales );
  
  // For antiga, no global parameters (like jerman's lambda_max)
  // we still have to mask the filter's response though
  
  using imageWriterType = ImageType;
  typedef  itk::ImageFileWriter< imageWriterType  > WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputVolume );
  
  typedef itk::Image<uint8_t, Dimension> MaskImageType;
  if( !maskVolume.empty() )
  {
    auto maskReader = MaskReaderType::New();
    maskReader->SetFileName(maskVolume);
    maskReader->Update();
    
    auto maskFilter = itk::MaskImageFilter<ImageType,MaskImageType>::New();
    maskFilter->SetInput( multiScaleEnhancementFilter->GetOutput() );
    maskFilter->SetMaskImage( maskReader->GetOutput() );
    maskFilter->SetMaskingValue(0);
    maskFilter->SetOutsideValue(0);
    
    maskFilter->Update();
    
    writer->SetInput( maskFilter->GetOutput() );
  }
  else
  {
    writer->SetInput( multiScaleEnhancementFilter->GetOutput() );
  }
  
  writer->Update();
  
  return EXIT_SUCCESS;
}


    /// Get the PixelType and ComponentType from fileName
  /// Copied from https://github.com/Slicer/Slicer/blob/master/Base/CLI/itkPluginUtilities.h
  void GetImageType(const std::string &fileName,
                    itk::ImageIOBase::IOPixelType &pixelType,
                    itk::ImageIOBase::IOComponentType &componentType) {
      typedef itk::Image<unsigned char, 3> ImageType;
      itk::ImageFileReader<ImageType>::Pointer imageReader =
              itk::ImageFileReader<ImageType>::New();
      imageReader->SetFileName(fileName);
      imageReader->UpdateOutputInformation();

      pixelType = imageReader->GetImageIO()->GetPixelType();
      componentType = imageReader->GetImageIO()->GetComponentType();
  }
}


int main(int argc, char *argv[]) {
    try {

        PARSE_ARGS;

        itk::ImageIOBase::IOPixelType pixelType;
        itk::ImageIOBase::IOComponentType componentType;
        GetImageType(inputVolume, pixelType, componentType);

        switch (componentType) {
            case itk::ImageIOBase::UCHAR:
                return DoIt<unsigned char>(argc, argv);
            case itk::ImageIOBase::CHAR:
                return DoIt<char>(argc, argv);
            case itk::ImageIOBase::USHORT:
                return DoIt<unsigned short>(argc, argv);
            case itk::ImageIOBase::SHORT:
                return DoIt<short>(argc, argv);
            case itk::ImageIOBase::UINT:
                return DoIt<unsigned int>(argc, argv);
            case itk::ImageIOBase::INT:
                return DoIt<int>(argc, argv);
            case itk::ImageIOBase::ULONG:
                return DoIt<unsigned long>(argc, argv);
            case itk::ImageIOBase::LONG:
                return DoIt<long>(argc, argv);
            case itk::ImageIOBase::FLOAT:
                return DoIt<float>(argc, argv);
            case itk::ImageIOBase::DOUBLE:
                return DoIt<double>(argc, argv);
            case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            default:
                throw std::exception();
        }
    }
    catch (std::exception &exception) {
        std::cerr << argv[0] << ": exception caught !" << std::endl;
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
}