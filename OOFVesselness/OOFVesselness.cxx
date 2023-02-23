/*
Author : Jonas lamy
Based on work of Turetken & Fethallah Benmansour
*/

#include "itkOptimallyOrientedFlux_GM.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkCastImageFilter.h"

#include "OOFVesselnessCLP.h"

namespace {
  template<class T>
  int DoIt(int argc, char *argv[]) {
          PARSE_ARGS;
    
    constexpr unsigned int Dimension = 3;
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

    using HessianPixelType = itk::SymmetricSecondRankTensor<double, Dimension>;
    using HessianImageType = itk::Image<HessianPixelType, Dimension>;
    
      //********************************************************
      //                   Filter
      //********************************************************

      using GaussianFilterType = itk::DiscreteGaussianImageFilter<ImageType,ImageType>;
      auto gFilter = GaussianFilterType::New();
      gFilter->SetVariance(Sigma);
      gFilter->SetInput(image);

      // creating radii from parameters

      double step =  ( std::log(sigmaMax) - std::log(sigmaMin) ) / (double)(numberOfScales-1);
      
      std::vector<double> radii;
      
      for(int level=0; level<numberOfScales;level++)
      {
        radii.push_back(sigmaMin * std::exp(step * level));
      }

      std::cout<<"scales: ";
      for(auto r :radii )
        std::cout<<r<<" ; ";
      std::cout<<std::endl;


      auto OOFfilter = itk::OptimallyOrientedFlux_GM<ImageType,ImageType>::New();
      OOFfilter->SetInput( gFilter->GetOutput() );
      OOFfilter->SetRadii(radii);
      try{
          itk::TimeProbe timer;
          timer.Start();
          //
          OOFfilter->Update();

          //
          timer.Stop();
          std::cout<<"Computation time:"<<timer.GetMean()<<std::endl;
      }
      catch(itk::ExceptionObject &e)
      {
          std::cerr << e << std::endl;
      }

    
      auto rescaleFilter = itk::RescaleIntensityImageFilter<ImageType>::New();
      

      if (!maskVolume.empty()) {
        auto maskReader = MaskReaderType::New();
        maskReader->SetFileName(maskVolume);
        maskReader->Update();
      
        auto maskFilter = itk::MaskImageFilter<ImageType,MaskImageType>::New();
        maskFilter->SetInput( OOFfilter->GetOutput() );
        maskFilter->SetMaskImage(maskReader->GetOutput());
        maskFilter->SetMaskingValue(0);
        maskFilter->SetOutsideValue(0);

        maskFilter->Update();
        rescaleFilter->SetInput( maskFilter->GetOutput() );
      }
      else{
        rescaleFilter->SetInput(OOFfilter->GetOutput());
      }
    
      rescaleFilter->SetOutputMinimum(0);
      rescaleFilter->SetOutputMaximum(1);
      

      typedef itk::ImageFileWriter<ImageType> ScoreImageWriter;
      auto writer = ScoreImageWriter::New();
      writer->SetInput( rescaleFilter->GetOutput() );
      writer->SetFileName(outputVolume);

      try{
          writer->Update();
      }
      catch(itk::ExceptionObject &e)
      {
          std::cerr << e << std::endl;
      }

      return 0;
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
