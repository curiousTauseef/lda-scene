/*************************************************************************
 *                                                                       *
 * Copyright (C) 2015, Navid Khazaee (nkh)                               *
 *                                                                       *
 * Email: navid.khazaee@gmail.com                                        *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with BASM. If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                       *
 *************************************************************************/

#include <cstdio>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <string>
#include <boost/filesystem.hpp>
#include <iterator>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace cv;

extern "C" {
      #include <vl/generic.h>
      #include <vl/dsift.h>
}

#define MIN_BINS 10
#define MAX_BINS 30

bool nkhImread(Mat &dst, string fileName, bool color=false)
{
    dst = imread(fileName);
    if(dst.empty())
    {
        cerr<< "Image Not Found! : " << fileName << endl;
        return false;
    }
    else
    {
        if(!color)
            cvtColor( dst, dst, CV_BGR2GRAY );
        return true;
    }
}

Mat getDescriptors(Mat img)
{
    //choose step & binSize
    srand (time(NULL));
    int binSize= (rand()%(MAX_BINS+1 - MIN_BINS))+ MIN_BINS;
    VlDsiftFilter *vlf = vl_dsift_new_basic(img.size().width, img.size().height, binSize/* 4 */ , binSize);//step, bin
    // transform image in cv::Mat to float vector
    std::vector<float> imgvec;
    for (int i = 0; i < img.rows; ++i){
        for (int j = 0; j < img.cols; ++j){
            imgvec.push_back(img.at<unsigned char>(i,j) / 255.0f);
        }
    }
    // call processing function of vl
    vl_dsift_process(vlf, &imgvec[0]);
    // echo number of keypoints found
    /* cout << "num of patches: " << vl_dsift_get_keypoint_num(vlf) << 
                  " patch size: " << 4*binSize << endl; */

    //cout << "size is : " << vl_dsift_get_descriptor_size(vlf) << endl;
    int numKeys = vl_dsift_get_keypoint_num(vlf);
    int descrDim = vl_dsift_get_descriptor_size(vlf);

    float* arr = new float[descrDim * numKeys];
    copy(vl_dsift_get_descriptors(vlf), vl_dsift_get_descriptors(vlf) + numKeys*descrDim, arr);
    Mat result = Mat(numKeys, descrDim, CV_32F, arr);
    return result;
}

int main(int argc, char *argv[])
{
    if (argc == 5) 
    {
        bool genYaml = (string(argv[4])=="--yaml") ? true : false ;
        Mat img;
        if (nkhImread(img, string(argv[1]))) //if image exists
        {
            //preprocess
            //GaussianBlur( img, img, Size(5, 5), 1,1 );
            //normalize(img,img,0,255,CV_MINMAX);

            //*************************** nkhStart BOW ***************************/ 

            //prepare BOW descriptor extractor from the dictionary    
            Mat vocabulary; 
            FileStorage fs(argv[3], FileStorage::READ);
            fs["vocabulary"] >> vocabulary;
            fs.release();    

            //create a nearest neighbor matcher
            Ptr<DescriptorMatcher> dmatcher(new FlannBasedMatcher);
            //Set the dictionary with the vocabulary we created in the first step
            CV_Assert( !vocabulary.empty() );
            dmatcher->add( std::vector<Mat>(1, vocabulary) );
            
            //get SIFT descriptor on patches in whole image, with random size again
            Mat keypointDescriptors;
            keypointDescriptors = getDescriptors(img);

            //To store the BoW (or BoF) representation of the image
            Mat bowDescriptor;        

            //********************** nkhStart compute bowDescriptor **********************//
            //extract BoW (or BoF) descriptor from given image : ported from opencv source
            //BOWImgDescriptorExtractor::
            //        compute( _descriptors, imgDescriptor, pointIdxsOfClusters i)

            int clusterCount  = vocabulary.rows;

            // Match keypoint descriptors to cluster center (to vocabulary)
            std::vector<DMatch> matches;
            dmatcher->match( keypointDescriptors, matches );

            // Compute image descriptor
            std::vector<std::vector<int> > pointIdxsOfClusters;
            pointIdxsOfClusters.resize(clusterCount);

            bowDescriptor.create(1, clusterCount, CV_32FC1); 
            bowDescriptor.setTo(Scalar::all(0));

            float *dptr = bowDescriptor.ptr<float>();
            for( size_t i = 0; i < matches.size(); i++ )
            {
                int queryIdx = matches[i].queryIdx;
                int trainIdx = matches[i].trainIdx; // cluster index
                CV_Assert( queryIdx == (int)i );

                dptr[trainIdx] = dptr[trainIdx] + 1.f;
                if( &pointIdxsOfClusters )
                    pointIdxsOfClusters[trainIdx].push_back( queryIdx );
            }
            
            //print Blei fromat for Latent Dirichlet Allocation (LDA) by Daichi Mochihashi
            //available at http://chasen.org/~daiti-m/dist/lda/ 
            for( int i = 0; i < clusterCount; i++ )
            {
                if(dptr[i] != 0)
                {
                    printf("%d:%d ", i+1, pointIdxsOfClusters[i].size());
                }
            }
            printf("\n");
            // Normalize image descriptor.
           // bowDescriptor /= keypointDescriptors.size().height;
            //********************** nkhEnd compute bowDescriptor **********************//
            if(genYaml)
            {
                //prepare the yml (some what similar to xml) file
                boost::filesystem::path filePath(argv[1]);
                //To store the image tag name - only for save the descriptor in a file
                string imageTag =  string(argv[2]) + "_" + filePath.stem().string();
                //open the file to write the resultant descriptor
                FileStorage fs1(string(imageTag + ".yml"), FileStorage::WRITE);    
                //write the new BoF descriptor to the file
                fs1 << imageTag << bowDescriptor;   

                //use this descriptor for classifying the image.
                //release the file storage
                fs1.release();
            }
            //*************************** nkhEnd BOW ***************************/ 

        }
        else
        {
            cerr << "file " << argv[1] << " does not exist!\n" ;
        }
    }
    else
    {
        printf("The usage is %s ImageFile label dictionary.yml --yaml/--no-yaml \n label is used to avoid confusion in repeated file names.\n",argv[0]);
    }
    return 0;
}

