#ifndef __LEARNOPENGL_COMMON_FRAMBUFFER_H__
#define __LEARNOPENGL_COMMON_FRAMBUFFER_H__

#include <stdio.h>
#include <GLFW/glfw3.h>

#include "glad/glad.h"

class Framebuffer {
 public:
  Framebuffer(int width, int height) {
    GLint save_id;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &save_id);

    glGenFramebuffers(1, &framebuffer_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);

    glGenTextures(1, &texture_buffer_id_);
    glBindTexture(GL_TEXTURE_2D, texture_buffer_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture_buffer_id_, 0);

    glGenRenderbuffers(1, &render_buffer_id_);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_id_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, render_buffer_id_);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    // restore
    glBindFramebuffer(GL_FRAMEBUFFER, save_id);
  }

  GLuint GetTextureId() { return texture_buffer_id_; }

  GLint Enable() {
    GLint save_id;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &save_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
  }

  void Disable(GLint id) {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
  }

 private:
  GLuint framebuffer_id_, texture_buffer_id_, render_buffer_id_;
};

#endif  // __LEARNOPENGL_COMMON_FRAMBUFFER_H__
