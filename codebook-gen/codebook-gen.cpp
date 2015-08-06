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
using namespace std;
using namespace cv;

extern "C" {
      #include <vl/generic.h>
      #include <vl/dsift.h>
}


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
    //TODO
    int binSize= 3;
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
    //cout << vl_dsift_get_keypoint_num(vlf) << endl;
    //cout << "size is : " << vl_dsift_get_descriptor_size(vlf) << endl;
    int numKeys = vl_dsift_get_keypoint_num(vlf);
    int descrDim = vl_dsift_get_descriptor_size(vlf);

    float* arr = new float[descrDim * numKeys];
    copy(std::begin(vl_dsift_get_descriptors(vlf)), end(vl_dsift_get_descriptors(vlf)), begin(arr));
    Mat result = Mat(descrDim, numKeys, CV_32F, arr);
    return result;
}

int main(int argc, char *argv[])
{
    if (argc == 2) 
    {

        for ( boost::filesystem::recursive_directory_iterator end, dir(argv[1]);
                dir != end; ++dir ) {
            Mat img;
            if (nkhImread(img, dir->path().string()))
            {
                //imshow(dir->path().filename().string(), img);
                //add patches descriptors to unclustered codebook 
                getDescriptors(img);
            }
            //generate sifts
            //BOW Train
        }
    }
    else
    {
        printf("The usage is %s .\n",argv[0]);
    }
    return 0;
}

