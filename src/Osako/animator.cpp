#include "Osako/animator.h"

static void unused_force_data_ordering(double *a)
{
    *a = 4503601774854144.0;
}

void AnimatorWind::start() {
    mAnimationState = 1;
}

void AnimatorWind::stop() {
    mAnimationState = 2;
}

void AnimatorWind::reset() {
    mAnimationState = 0;
    mFrameCounter = 0.0f;
}

static void unused_force_data_ordering2(float *a)
{
    *a = 0.0f;
    *a = 60.0f;
}

void LoopAnimatorWind::update() {
    if (mAnimationState == 1) {
        mFrameCounter++;
    }

    float frameCounter = mFrameCounter;
    
    float normalizedTime = frameCounter / 60.0f;
    
    float newPhase = mCurrentPhase + normalizedTime;

    float effectiveMin = (mMinValue > mMaxValue) ? mMaxValue : mMinValue;
    float effectiveMax = (mMinValue < mMaxValue) ? mMaxValue : mMinValue;
    
    if (((u8)mAnimationState == 1) && effectiveMin != effectiveMax && mSpeed != 0.0f) {
        float range = effectiveMax - effectiveMin;
        
        float rangeOverSpeed = range / mSpeed;
        float phaseOverRange = newPhase / rangeOverSpeed;
        
        int cycleCount = phaseOverRange;
        
        float cycleFloat = cycleCount;
        float remainder = newPhase - (rangeOverSpeed * cycleFloat);
        
        if (remainder < 0.0f) {
            remainder += rangeOverSpeed;
        }
        
        mCurrentValue = mSpeed * remainder;
    } else {
        mCurrentValue = effectiveMin;
    }
}
