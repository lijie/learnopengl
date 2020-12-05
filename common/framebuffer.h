#ifndef __LEARNOPENGL_COMMON_FRAMBUFFER_H__
#define __LEARNOPENGL_COMMON_FRAMBUFFER_H__

#include "lo_common.h"
#include <stdio.h>

class Framebuffer {
 public:
  Framebuffer(int width, int height) {
    int save_id;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &save_id);

    width_ = width;
    height_ = height;

    // InitFramebufferTexture(width, height);
    // if (sample > 0) InitMultisampleFramebufferTexture(width, height, sample);
    printf("init framebuffer %d, %d\n", width, height);
#if 0
    Init();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      fprintf(stderr, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    // restore
    glBindFramebuffer(GL_FRAMEBUFFER, save_id);
#endif
  }

  virtual ~Framebuffer() {}

  virtual void Init() = 0;

  virtual int GetTextureId() { return -1; }
  virtual int GetFramebufferId() = 0;

  virtual void Enable() {
    int save_id;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &save_id);
    if (GetFramebufferId() < 0) {
      Init();
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
      }
    }
    // printf("bind framebuffer: %d\n", GetFramebufferId());
    glBindFramebuffer(GL_FRAMEBUFFER, GetFramebufferId());
    last_framebuffer_id_ = save_id;
  }

  virtual void Blit() {
  //   if (sample_ > 0) {
  //     glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_ms_id_);
  //     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id_);
  //     glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_,
  //                       GL_COLOR_BUFFER_BIT, GL_NEAREST);
  //   }
  }

  void Disable() { glBindFramebuffer(GL_FRAMEBUFFER, last_framebuffer_id_); }

 protected:
  // unsigned int framebuffer_id_, texture_buffer_id_, render_buffer_id_;
  // unsigned int framebuffer_ms_id_, texture_buffer_ms_id_, render_buffer_ms_id_;
  int last_framebuffer_id_ = 0;
  int width_, height_;

#if 0
  void InitFramebufferTexture(int width, int height) {
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
  }
  void InitMultisampleFramebufferTexture(int width, int height, int sample) {
    glGenFramebuffers(1, &framebuffer_ms_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_ms_id_);
    glGenTextures(1, &texture_buffer_ms_id_);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_buffer_ms_id_);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, GL_RGB, width,
                            height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D_MULTISAMPLE, texture_buffer_ms_id_, 0);

    glGenRenderbuffers(1, &render_buffer_ms_id_);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_ms_id_);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample,
                                     GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, render_buffer_ms_id_);
  }
#endif
};

class TextureFramebuffer : Framebuffer {
 public:
  TextureFramebuffer(int width, int height) : Framebuffer(width, height) {}

  ~TextureFramebuffer() {
    glDeleteBuffers(1, &render_buffer_id_);
    glDeleteBuffers(1, &texture_buffer_id_);
    glDeleteBuffers(1, &framebuffer_id_);
  }

  void Init() override {
    glGenFramebuffers(1, &framebuffer_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
    glGenTextures(1, &texture_buffer_id_);
    glBindTexture(GL_TEXTURE_2D, texture_buffer_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture_buffer_id_, 0);

    glGenRenderbuffers(1, &render_buffer_id_);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_id_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, render_buffer_id_);
  }

  private:
    unsigned int framebuffer_id_, texture_buffer_id_, render_buffer_id_;
};

#if 0
class MultisampleTextureFramebuffer : Framebuffer {
 public:
  MultisampleTextureFramebuffer(int width, int height, int sample)
      : Framebuffer(width, height) {}

  ~MultisampleTextureFramebuffer() {
    glDeleteBuffers(1, &render_buffer_id_);
    glDeleteBuffers(1, &texture_buffer_id_);
    glDeleteBuffers(1, &framebuffer_id_);
  }

  void Blit() override {
    if (sample_ > 0) {
      glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_ms_id_);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id_);
      glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
  }

  private:
    unsigned int framebuffer_id_, texture_buffer_id_, render_buffer_id_;
};
#endif

class DepthFramebuffer : public Framebuffer {
 public:
  DepthFramebuffer(int width, int height) : Framebuffer(width, height) {}

void Init() override {
    glGenFramebuffers(1, &framebuffer_id_);
    CHECK_GL_ERROR;
    printf("Gen Framebuffer %d\n", framebuffer_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
    CHECK_GL_ERROR;
    glGenTextures(1, &texture_buffer_id_);
    CHECK_GL_ERROR;
    printf("Gen Texture %d\n", texture_buffer_id_);
    glBindTexture(GL_TEXTURE_2D, texture_buffer_id_);
    CHECK_GL_ERROR;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width_, height_, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, NULL);
                 CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    CHECK_GL_ERROR;
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
    //                        texture_buffer_id_, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_buffer_id_, 0);
    CHECK_GL_ERROR;
    glDrawBuffer(GL_NONE);
    CHECK_GL_ERROR;
    // glReadBuffer(GL_NONE);

    // glGenRenderbuffers(1, &render_buffer_id_);
    // glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_id_);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
    //                           GL_RENDERBUFFER, render_buffer_id_);
  }

  void Enable() override {
    int save_id;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &save_id);
    CHECK_GL_ERROR;
    if (GetFramebufferId() < 0) {
      Init();
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
      }
    }
    // printf("bind framebuffer: %d\n", GetFramebufferId());
    glBindFramebuffer(GL_FRAMEBUFFER, GetFramebufferId());
    CHECK_GL_ERROR;
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_buffer_id_, 0);
    CHECK_GL_ERROR;
    last_framebuffer_id_ = save_id;
  }

  int GetTextureId() override { return texture_buffer_id_; }
  int GetFramebufferId() override { return framebuffer_id_; }

  private:
    unsigned int framebuffer_id_, texture_buffer_id_, render_buffer_id_;
};

#endif  // __LEARNOPENGL_COMMON_FRAMBUFFER_H__
