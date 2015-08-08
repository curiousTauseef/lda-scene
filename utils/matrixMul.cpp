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
#include <cstdlib>
using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    if (argc == 4) 
    {
        FileStorage alphaFS(argv[1], FileStorage::READ);
        FileStorage betaFS(argv[2], FileStorage::READ);
        FileStorage imgFS(argv[3], FileStorage::READ);

        boost::filesystem::path alphaPath(argv[1]);
        boost::filesystem::path betaPath(argv[2]);
        boost::filesystem::path imgPath(argv[3]);
        Mat alpha, beta, img;
        alphaFS[alphaPath.stem().string()] >> alpha;
        betaFS[betaPath.stem().string()] >> beta;
        imgFS[imgPath.stem().string()] >> img;

        
        printf("Alpha %d*%d , Beta %d*%d , Img %d*%d \n", alpha.rows, alpha.cols, beta.rows, beta.cols, 
                img.rows, img.cols);
        Mat temp = alpha * beta.t(); 
        Mat result = temp * img.t();
        cout << result << endl; 
        alphaFS.release();
        betaFS.release();
        imgFS.release();

    }
    else
    {
        printf("The usage is %s model.apha model.beta image.yml \n",argv[0] );
    }
    return 0;
}

