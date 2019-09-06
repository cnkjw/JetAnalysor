//
// Created by JinWen on 2019/4/30.
//

#ifndef JETANALYSOR_DISTCALCULATOR_H
#define JETANALYSOR_DISTCALCULATOR_H

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

namespace JetAnalysor {

    const static double PI = acos(-1);

    struct distInfo {
        double variable;
        double dist_value;
    };

    struct region {
        double leftValue;
        double rightValue;
    };

    bool isAtRegion(const region &mRegion, const double &value) {
        if (value >= mRegion.leftValue && value < mRegion.rightValue) {
            return true;
        } else {
            return false;
        }
    }

    class distCalculator {
        private:
            /* data */
            vector<distInfo> mDistInfoVector;
            vector<region> mRegionVector;
            double mNormalisationValue;
            int mSize;

            double mStartValue, mEndValue;

            int findAtRegin(double value);

        public:
            distCalculator(double start_value, double end_value, 
                double half_of_delta_value);

            inline int getSize() {
                return mSize;
            }
            inline double getVariable(int i) {
                return mDistInfoVector[i].variable;
            }
            inline double getDistValue(int i) {
                return mDistInfoVector[i].dist_value;
            }

            void clear();
            void addEventNum(const double &value, const double &weight);
            inline void addEventNorm(const double &weight);

            // 重载加法运算符
            distCalculator operator+(const distCalculator &mDist);

            // 获取 1/N_Z dN^{jZ}/d\delta\phi_{jZ} 的值列表
            vector<distInfo> getDistList();
    };
    
    distCalculator::distCalculator(double start_value, double end_value, 
        double half_of_delta_value) {
        //
        const static int vec_size = floor((end_value - start_value) / (
            2 * half_of_delta_value
        )) + 1;
        mDistInfoVector.resize(vec_size);
        mRegionVector.resize(vec_size);
        mDistInfoVector[0].variable = start_value + half_of_delta_value;
        mDistInfoVector[0].dist_value = 0.;
        mRegionVector[0].leftValue = start_value;
        mRegionVector[0].rightValue = mDistInfoVector[0].variable + half_of_delta_value;
        for (int i = 1; i < vec_size; i++) {
            mDistInfoVector[i].variable = mDistInfoVector[i - 1].variable 
                + 2 * half_of_delta_value;
            mDistInfoVector[i].dist_value = 0.;
            mRegionVector[i].leftValue = mRegionVector[i - 1].rightValue;
            mRegionVector[i].rightValue = mDistInfoVector[i].variable + half_of_delta_value;
        }
        if (mDistInfoVector.back().variable > end_value) {
            mDistInfoVector.back().variable = end_value;
            mRegionVector.back().rightValue = end_value;
        }
        mNormalisationValue = 0.;
        mSize = mDistInfoVector.size();
        mStartValue = start_value;
        mEndValue = end_value;
    }

    void distCalculator::addEventNum(const double &value, const double &weight) {
        //
        int regin_index = findAtRegin(value);
        mDistInfoVector[regin_index].dist_value += weight;
    }

    int distCalculator::findAtRegin(double value) {
        for (unsigned i = 0; i < mRegionVector.size(); i++) {
            if (isAtRegion(mRegionVector[i], value)) {
                return i;
            }
        }
        if (fabs(value - mEndValue) <= 1.0E-06) {
            return mRegionVector.size() - 1;
        } else {
            cout << "ERROR: you given value: " << value << " don't belong to any region." << endl;
        }
    }

    inline void distCalculator::addEventNorm(const double &weight) {
        mNormalisationValue += weight;
    }
    
    void distCalculator::clear() {
        for (auto mDistInfo : mDistInfoVector) {
            mDistInfo.dist_value = 0.0;
        }
        mNormalisationValue = 0.0;
    }

    distCalculator distCalculator::operator+(const distCalculator &mDist) {
        for (auto mDistVecComponent : mDist.mDistInfoVector) {
            (this->mDistInfoVector[findAtRegin(mDistVecComponent.variable)]).dist_value 
                += mDistVecComponent.dist_value;
        }
        this->mNormalisationValue += mDist.mNormalisationValue;
        return *this;
    }

    // 获取 1/N_Z dN^{jZ}/d\delta\phi_{jZ} 的值列表
    vector<distInfo> distCalculator::getDistList() {
        vector<distInfo> temp;
        temp = mDistInfoVector;
        for (unsigned i = 0; i < mSize; i++) {
            temp[i].dist_value = mDistInfoVector[i].dist_value / 
                (mRegionVector[i].rightValue - mRegionVector[i].leftValue) / 
                mNormalisationValue;
        }
        return temp;
    }
}

#endif // JETANALYSOR_DISTCALCULATOR_H