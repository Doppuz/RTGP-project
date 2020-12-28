#pragma once;

#include <string>

#include <glad/glad.h>

#include <glfw/glfw3.h>

class WaterFrameBuffers {

public:

	GLuint REFLECTION_WIDTH = 320;
	GLuint REFLECTION_HEIGHT = 180;

	GLuint REFRACTION_WIDTH = 1280;
	GLuint REFRACTION_HEIGHT = 720;

	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;

	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

	WaterFrameBuffers() {	// to be called when loading the program
		initialiseReflectionFrameBuffer();
		initialiseRefractionFrameBuffer();
	}

	void cleanUp() {	//to be called when closing the program
		glDeleteFramebuffers(1, &reflectionFrameBuffer);
		glDeleteTextures(1, &reflectionTexture);
		glDeleteRenderbuffers(1, &reflectionDepthBuffer);
		glDeleteFramebuffers(1, &refractionFrameBuffer);
		glDeleteTextures(1, &refractionTexture);
		glDeleteTextures(1, &refractionDepthTexture);
	}

	void bindReflectionFrameBuffer() {//call before rendering to this FBO
		bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
	}

	void bindRefractionFrameBuffer() {//call before rendering to this FBO
		bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
	}

	void unbindCurrentFrameBuffer() {//call to switch to default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1600, 900);
	}

	int getReflectionTexture() {//get the resulting texture
		return reflectionTexture;
	}

	int getRefractionTexture() {//get the resulting texture
		return refractionTexture;
	}

	int getRefractionDepthTexture() {//get the resulting depth texture
		return refractionDepthTexture;
	}

	void initialiseReflectionFrameBuffer() {
		reflectionFrameBuffer = createFrameBuffer();
		reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		unbindCurrentFrameBuffer();
	} 

	void initialiseRefractionFrameBuffer() {
		refractionFrameBuffer = createFrameBuffer();
		refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		unbindCurrentFrameBuffer();
	}

	void bindFrameBuffer(int frameBuffer, int width, int height) {
		glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glViewport(0, 0, width, height);
		//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height); this didn't fix it...
	}

	GLuint createFrameBuffer() {
		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		//generate name for frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		//create the framebuffer
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//indicate that we will always render to color attachment 0
		return frameBuffer;
	}

	GLuint createTextureAttachment(int width, int height) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //(ByteBuffer)null
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //?
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //?
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB tex error, status: 0x%x\n", Status);
			return false;
		}

		return texture;
	}

	GLuint createDepthTextureAttachment(int width, int height) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); //(ByteBuffer)null
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB depth error, status: 0x%x\n", Status);
			return false;
		}

		return texture;
	}

	GLuint createDepthBufferAttachment(int width, int height) {
		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		return depthBuffer;
	}

};