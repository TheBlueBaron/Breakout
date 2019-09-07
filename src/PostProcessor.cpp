#include "PostProcessor.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, GLuint width, GLuint height)
	: postProcessingShader(shader), texture(), width(width), height(height), confuse(GL_FALSE), chaos(GL_FALSE), shake(GL_FALSE) {
	// Initialise renderbuffer / frambuffer object
	glGenFramebuffers(1, &this->MSFBO);
	glGenFramebuffers(1, &this->FBO);
	glGenRenderbuffers(1, &this->RBO);

	// Initialise renderbuffer storage with a multisampled color buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, width, height); // Allocate storage for render buffer object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); // Attach MS render buffer object to frambuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialse MSFBO" << std::endl;
	}

	// Also initialise the FBO / texture to multisampled color-buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	this->texture.generate(width, height, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture.ID, 0); // Attach texture to framebuffer as its color attachment
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialise FBO" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Initialise render data and uniforms
	this->initRenderData();
	this->postProcessingShader.setInteger("scene", 0, GL_TRUE);
	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset,  offset },
		{  0.0f,    offset },
		{  offset,  offset },
		{ -offset,  0.0f   },
		{  0.0f,    0.0f   },
		{  offset,  0.0f   },
		{ -offset, -offset },
		{  0.0f    -offset },
		{  offset, -offset }
	};
	glUniform2fv(glGetUniformLocation(this->postProcessingShader.ID, "offsets"), 9, (GLfloat*)offsets);
	GLint edgeKernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(this->postProcessingShader.ID, "edgeKernel"), 9, edgeKernel);
	GLfloat blurKernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv(glGetUniformLocation(this->postProcessingShader.ID, "blurKernel"), 9, blurKernel);
}

void PostProcessor::beginRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::endRender() {
	// Now resolve multisampled color-buffer into intermediate FBO to store texture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, this->width, this->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcessor::render(GLfloat time) {
	// Set unifomrs / options
	this->postProcessingShader.use();
	this->postProcessingShader.setFloat("time", time);
	this->postProcessingShader.setInteger("confuse", this->confuse);
	this->postProcessingShader.setInteger("chaos", this->chaos);
	this->postProcessingShader.setInteger("shake", this->shake);
	// Render texture quad
	glActiveTexture(GL_TEXTURE0);
	this->texture.bind();
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData() {
	// Configure VAO / VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos		  // Tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
