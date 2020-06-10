#include <FrameBufferGUI.hpp>


FrameBufferGUI::FrameBufferGUI(){
    this->cbo = 0;
    this->rbo = 0;
    this->fbo = 0;
}

bool FrameBufferGUI::Generate(GLint width, GLint height, GLenum textureUnitGUI){
    Delete();
    DEBUG_GLCHECK( glGenFramebuffers(1, &this->fbo); );
    DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, this->fbo); );
        // Colorbuffer
        DEBUG_GLCHECK( glGenTextures(1, &this->cbo); );
        DEBUG_GLCHECK( glActiveTexture(textureUnitGUI); );
        DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, this->cbo); );
        DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); );
        DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
        DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
        DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
        DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
        DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->cbo, 0); );
        // Rendering buffer for depth
        DEBUG_GLCHECK( glGenRenderbuffers(1, &this->rbo); );
        DEBUG_GLCHECK( glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);  );
        DEBUG_GLCHECK( glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); );
        DEBUG_GLCHECK( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo); );
        // Check for completeness
        if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)){
            Delete();
            return false;
        }
    DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0); );
    return true;
}

void FrameBufferGUI::Delete(void){
    // Delete colorbuffers
    if(this->cbo){
        glDeleteTextures(1, &this->cbo);
        this->cbo = 0;
    }

    // Delete renderbuffer
    if(this->rbo){
        glDeleteRenderbuffers(1, &this->rbo);
        this->rbo = 0;
    }

    // Delete framebuffer
    if(this->fbo){
        glDeleteFramebuffers(1, &this->fbo);
        this->fbo = 0;
    }
}

