#include "Camera.h"

Camera::Camera(float sensitivity)
	   :up(glm::vec3(0.0f, 1.0f, 0.0f)),
		position(glm::vec3(0.0f, 0.0f, 0.0f)),
		pitch(0.0f),
		yaw(-90.0f),
		sensitivity(sensitivity)
	{
	updateDirection();
}

glm::vec3 Camera::getRight() {
	
	glm::vec3 s = glm::cross(direction, up);
	return s;
}

void Camera::updateDirection() {
	direction = 
	glm::normalize(glm::vec3(
		glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
		glm::sin(glm::radians(pitch)),
		glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))
	));
}


glm::mat4 Camera::getView() {

	glm::mat4 trans(1.0f);

	glm::vec3 right = getRight();
	glm::vec3 camup = glm::cross(right, direction); //bcz direction is flipped
	glm::mat4 rotat = glm::mat4(
		glm::vec4(right, 0.0f),
		glm::vec4(camup, 0.0f),
		glm::vec4(-direction, 0.0f), // so that it doesn't flip the axes of the objs
		glm::vec4(glm::vec3(0.0f), 1.0f)
	);

	rotat = glm::transpose(rotat); // inv(A) = A^T if a is orthonormal

	trans = glm::translate(trans, -position);
	//view = glm::rotate(view, -yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	//view = glm::rotate(view, -pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	return rotat*trans;
}
void Camera::handleMovement(GLFWwindow* window, float dt) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += direction * dt;

	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= direction * dt;

	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += getRight() * dt;

	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= getRight() * dt;
}

void Camera::move(direc d, float dt) {
	switch (d) {
		case UP:
			position += direction * dt;
			break;
		case DOWN:
			position -= direction * dt;
			break;
		case RIGHT:
			position += getRight() * dt;
			break;
		case LEFT:
			position -= getRight() * dt;
			break;
	}
}

void Camera::incPitch(float increment) {
	this->pitch += increment;
	this->pitch = funcs::clamp(this->pitch, -45.0f, 45.0f);
	updateDirection();
}

void Camera::incYaw(float increment) {
	this->yaw += increment;
	yaw = (yaw > 360) ? (yaw - 360) : (yaw < -360) ? yaw + 360 : yaw;
	// this->yaw = funcs::clamp(this->yaw, -360.0f, 360.0f);
	updateDirection();
}

void Camera::handleMouse(double xPos, double yPos) {
	if (firstMouse) {
		lxpos = xPos;
		lypos = yPos;
		firstMouse = false;
		return;
	}

	float dx = xPos - lxpos;
	float dy = lypos - yPos;

	yaw += dx * sensitivity;
	pitch += dy * sensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;
	yaw = (yaw > 360) ? (yaw - 360) : (yaw < -360) ? yaw + 360 : yaw;

	updateDirection();

	lxpos = xPos;
	lypos = yPos;
}
