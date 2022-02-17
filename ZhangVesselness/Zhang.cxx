#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkHessianToRuiZhangMeasureImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageRegionIterator.h"

#include "itkStatisticsImageFilter.h"

#include "itkKdTree.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkWeightedCentroidKdTreeGenerator.h"

#include "itkMinimumDecisionRule.h"
#include "itkEuclideanDistanceMetric.h"
#include "itkImageToListSampleAdaptor.h"

#include "itkCastImageFilter.h"

#include <string>
#include "itkTimeProbe.h"

#include "ZhangCLP.h"

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

  using CastType = itk::CastImageFilter<InputImageType, ImageType>;
  using HessianPixelType = itk::SymmetricSecondRankTensor<double, Dimension>;
  using HessianImageType = itk::Image<HessianPixelType, Dimension>;

  auto reader = ReaderType::New();
  reader->SetFileName(inputVolume);
  auto cast = CastType::New();
  cast->SetInput(reader->GetOutput());
  cast->Update();
  auto img = cast->GetOutput();

  auto duplicator = itk::ImageDuplicator<ImageType>::New();
  auto stats = itk::StatisticsImageFilter<ImageType>::New();
  
  typedef itk::Statistics::ImageToListSampleAdaptor< ImageType >   AdaptorType;
  AdaptorType::Pointer adaptor = AdaptorType::New();
  
  MaskImageType::Pointer maskImage;
  ImageType::Pointer maskedImage;

  if( !maskVolume.empty() )
  {
    auto maskReader = MaskReaderType::New();
    maskReader->SetFileName(maskVolume);
    maskReader->Update();

    maskImage = maskReader->GetOutput();
    // creating the mask manually.

    duplicator->SetInputImage(img);
    duplicator->Update();
    maskedImage = duplicator->GetOutput();

    itk::ImageRegionIterator<ImageType> itMasked(maskedImage,maskedImage->GetLargestPossibleRegion());
    itk::ImageRegionIterator<MaskImageType> itMask(maskImage,maskedImage->GetLargestPossibleRegion());
    itMask.GoToBegin();
    itMasked.GoToBegin();
    
    while(!itMasked.IsAtEnd())
    {
      if( itMask.Get() == 0)
      {
        itMasked.Set(0);
      }
      ++itMasked;
      ++itMask;
    }
    
    adaptor->SetImage( maskedImage );
    stats->SetInput( maskedImage );
  }
  else
  {
    adaptor->SetImage( img );
    stats->SetInput( img );
  }
  
  // TODO : stats hangs when masked option is passed....
  // The function works well without masks, suggesting that the duplicator... is at fault...

  stats->Update();
  adaptor->Update();


    // K-d tree structure
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator< AdaptorType > TreeGeneratorType;
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();
  typedef TreeGeneratorType::KdTreeType TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;
  EstimatorType::Pointer estimator = EstimatorType::New();
  
  
  double min = stats->GetMinimum(); // 0/4
  double max = stats->GetMaximum(); // 4/4
  
  EstimatorType::ParametersType initialMeans(nbClasses);
  
  std::cout<<"min seed: "<<min<<std::endl;
  
  initialMeans[0] = min;
  double step = (max - min) / nbClasses;
  for(int i=1; i<nbClasses-1;i++)
  {
    initialMeans[i] = step * i + min;
    std::cout<<"seed: "<<step*i+min<<std::endl;
  }
  initialMeans[nbClasses-1] = max;
  
  std::cout<<"max seed: "<<max<<std::endl;
  itk::TimeProbe clock;
  
  clock.Start();
  treeGenerator->SetSample( adaptor );
  treeGenerator->SetBucketSize( 16 );
  treeGenerator->Update();
  estimator->SetParameters( initialMeans );
  estimator->SetKdTree( treeGenerator->GetOutput() );
  estimator->SetMaximumIteration( 200 );
  estimator->SetCentroidPositionChangesThreshold(0.0);
  estimator->StartOptimization();
  clock.Stop();
  
  std::cout<<"clock:"<<clock.GetTotal()<<std::endl;
  EstimatorType::ParametersType estimatedMeans = estimator->GetParameters();
  
  for ( unsigned int i = 0 ; i < nbClasses ; ++i )
  {
    std::cout << "cluster[" << i << "] " << std::endl;
    std::cout << "    estimated mean : " << estimatedMeans[i] << std::endl;
  }
  
  double alpha = (estimatedMeans[nbClasses-1] - estimatedMeans[nbClasses-2])/2.0;
  double beta  = (estimatedMeans[nbClasses-1] + estimatedMeans[nbClasses-1])/2.0;
  
  // filtering with sigmoid
  
  typedef itk::SigmoidImageFilter<ImageType,ImageType> SigmoidFilterType;
  auto sigmoidFilter = SigmoidFilterType::New();
  
  sigmoidFilter->SetOutputMaximum(1);
  sigmoidFilter->SetOutputMinimum(0);
  
  sigmoidFilter->SetAlpha(alpha);
  sigmoidFilter->SetBeta(beta);
  
  sigmoidFilter->SetInput( img );
  
  using HessianPixelType = itk::SymmetricSecondRankTensor< double, Dimension >;
  using HessianImageType = itk::Image< HessianPixelType, Dimension >;
  
  using OutputImageType = itk::Image< double, Dimension >;
  
  using RuiZhangFilterType = itk::HessianToRuiZhangMeasureImageFilter<HessianImageType, OutputImageType,MaskImageType>;
  auto ruiZhangFilter = RuiZhangFilterType::New();
  ruiZhangFilter->SetTau(Tau);
  
  if( !maskVolume.empty() )
  {
    ruiZhangFilter->SetMaskImage(maskImage);
  }
  
  using MultiScaleEnhancementFilterType = itk::MultiScaleHessianBasedMeasureImageFilter< ImageType, HessianImageType, OutputImageType >;
  MultiScaleEnhancementFilterType::Pointer multiScaleEnhancementFilter =  MultiScaleEnhancementFilterType::New();
  multiScaleEnhancementFilter->SetInput( sigmoidFilter->GetOutput() );
  multiScaleEnhancementFilter->SetHessianToMeasureFilter( ruiZhangFilter );
  multiScaleEnhancementFilter->SetSigmaStepMethodToLogarithmic();
  multiScaleEnhancementFilter->SetSigmaMinimum( sigmaMin );
  multiScaleEnhancementFilter->SetSigmaMaximum( sigmaMax );
  multiScaleEnhancementFilter->SetNumberOfSigmaSteps( numberOfScales );
  
  // Saving image
  
  using imageWriterType = ImageType;
  typedef  itk::ImageFileWriter< imageWriterType  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( multiScaleEnhancementFilter->GetOutput() );
  writer->SetFileName( outputVolume );
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
