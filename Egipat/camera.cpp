#include "camera.hpp"

#include <glm/gtc/constants.hpp>


OrbitalCamera::OrbitalCamera(float fov, float distance, float rotateSpeed, float moveSpeed, const glm::vec3 &worldUp, const glm::vec3 &target)
    : mWorldUp(worldUp),
      mTarget(target)  {
    mFOV = fov;
    mRadius = distance;
    mMoveSpeed = moveSpeed;
    mRotateSpeed = rotateSpeed;
    mWorldUp = worldUp;
    mTarget = target;
    mYaw = -90.0f;
    mPitch = 0.0f;
    updateVectors();
}

void
OrbitalCamera::Rotate(float dYaw, float dPitch, float dt) {
    dYaw *= mRotateSpeed * dt;
    dPitch *= mRotateSpeed * dt;

    mYaw += dYaw;
    mPitch += dPitch;

    if(mPitch > 89.0f)
        mPitch = 89.0f;
    if(mPitch < -89.0f)
        mPitch = -89.0f;

    updateVectors();
}

void
OrbitalCamera::Move(float dx, float dy, float dt) {
    float speed = mMoveSpeed;

    if (mMoveFaster)
        speed *= 2.5f;

    if ((dx == 0 or dy == 0) and dx != dy)
        speed = speed / sqrt(2.0f);

    mPosition += (dx * mRight + dy * mFront) * speed * dt;
    updateVectors();
}


void
OrbitalCamera::updateVectors() {
    mFront.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront.y = sin(glm::radians(mPitch));
    mFront.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront = glm::normalize(mFront);
    mRight = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));
    mTarget = mPosition + mFront;
}