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

#define MIN_BINS 3
#define MAX_BINS 8

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
    VlDsiftFilter *vlf = vl_dsift_new_basic(img.size().width, img.size().height, binSize*4 , binSize);//step, bin
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
    cout << "num of patches: " << vl_dsift_get_keypoint_num(vlf) << 
                  " patch size: " << 4*binSize << endl;

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
    if (argc == 2) 
    {
        Mat featuresUnclustered; 
        //generate descriptors for each image
        for ( boost::filesystem::recursive_directory_iterator end, dir(argv[1]);
                dir != end; ++dir ) {
            if( is_directory(dir->path()) )
                continue;

            Mat img;
            if (nkhImread(img, dir->path().string())) //if image exists
            {
                //add patches descriptors to unclustered codebook 
                cout << "Processing " << dir->path().string() << " ...\n" ; 
                featuresUnclustered.push_back(getDescriptors(img));
            }
            else
            {
                cerr << "file " << dir->path().string() << " does not exist!\n" ;
            }
        }
        //BOW Train
        int dictionarySize=200;
        //define Term Criteria
        TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
        //retries number
        int retries=1;
        //necessary flags
        int flags=KMEANS_PP_CENTERS;
        
        //Create the BoW (or BoF) trainer : Make sure you have enough images to
        // generate a number of pathces more than dictionarySize or you'll get an
        // Assertion failed (N >= K) in kmeans
        printf("Running KMeans for K=%d  ... \n", dictionarySize );
        BOWKMeansTrainer bowTrainer(dictionarySize,tc,retries,flags);
        //cluster the feature vectors
        Mat dictionary=bowTrainer.cluster(featuresUnclustered);    
        //store the vocabulary
        printf("Writing vocabulary to dictionary.yml ...\n");
        FileStorage fs("dictionary.yml", FileStorage::WRITE);
        fs << "vocabulary" << dictionary;
        fs.release();
        printf("Done!\n");
    }
    else
    {
        printf("The usage is %s trainDirectory \n",argv[0]);
    }
    return 0;
}

