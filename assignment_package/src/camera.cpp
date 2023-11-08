#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(400, 400)
{
    look = glm::vec3(0,0,-1);
    up = glm::vec3(0,1,0);
    right = glm::vec3(1,0,0);
    theta = .0f;
    phi = .0f;
    zoom = 10.0f;
    RecomputeAttributes();
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0), .0f, .0f, 10.0f)
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp, float t, float p, float z):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(100000),
    eye(e),
    ref(r),
    world_up(worldUp)
{
    zoom = glm::length(e);
    theta = atan2(e.z, e.x);
    phi = acos(e.y / zoom);
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H)
{
    zoom = glm::length(eye);
    theta = atan2(eye.z, eye.x);
    phi = acos(eye.y / zoom);
    RecomputeAttributes();
}


void Camera::RecomputeAttributes()
{
    zoom = glm::length(eye);
    theta = atan2(eye.z, eye.x);
    phi = acos(eye.y / zoom);

    look = glm::normalize(ref - eye);
    right = glm::normalize(glm::cross(look, world_up));
    up = glm::cross(right, look);

    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width / static_cast<float>(height);
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

glm::mat4 Camera::getViewProj()
{
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    theta += glm::radians(deg);
    // Compute eye position based on spherical coordinates
    eye = glm::vec3(
        zoom * sinf(phi) * cosf(theta),
        zoom * cosf(phi),
        zoom * sinf(phi) * sinf(theta)
        );
}
void Camera::RotateAboutRight(float deg)
{
    phi -= glm::radians(deg);
    phi = glm::clamp(phi, 0.01f, 3.14f);
    // Compute eye position based on spherical coordinates
    eye = glm::vec3(
        zoom * sinf(phi) * cosf(theta),
        zoom * cosf(phi),
        zoom * sinf(phi) * sinf(theta)
        );
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
}
