#include <camera.h>

Camera::Camera(vec3 position, vec3 up, GLfloat yaw, GLfloat pitch) :
Forward(vec3(0.0f,0.0f,-1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY),Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch): 
Forward(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY),Zoom(ZOOM) 
{
    Position = vec3(posX, posY, posZ);
    WorldUp = vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

mat4 
Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Forward, Up);
}

void 
Camera::ProcessKeyBoard(Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Forward * velocity;
    else if (direction == BACKWARD)
        Position -= Forward * velocity;
    else if (direction == LEFT)
        Position -= Right * velocity;
    else if (direction == RIGHT)
        Position += Right * velocity;
}

void
Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)///make sure the screen won't get flipped
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < - 89.0f)
            Pitch = -89.0f;
    }
    updateCameraVectors();
}

void
Camera::ProcessMouseScroll(GLfloat yoffset)
{
    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

void
Camera::updateCameraVectors()
{
    vec3 forward;
    forward.x = cos(radians(Yaw)) * cos(radians(Pitch));
    forward.y = sin(radians(Pitch));
    forward.z = sin(radians(Yaw)) * cos(radians(Pitch));
    Forward = normalize(forward);
    Right = normalize(cross(Forward, WorldUp));
    Up = normalize(cross(Right, Forward));
}