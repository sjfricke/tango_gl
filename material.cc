#include <tango-gl/shaders.h>
#include "tango-gl/material.h"

namespace tango_gl {

    Material::Material() {
        SetFallbackShader();
        util::CheckGlError("Material::ctor");
    }

    Material::~Material() {
        shader_program_ = 0;
        fallback_shader_program_ = 0;
    }

    bool Material::SetShader(const char *vertex_shader, const char *pixel_shader) {
        params_float_.clear();
        params_vec4_.clear();

        GLuint program = util::CreateProgram(vertex_shader, pixel_shader);
        if (!program) {
            SetFallbackShader();
            return false;
        }

        bool result = SetShaderInternal(program);
        if (!result) {
            SetFallbackShader();
        }
        return result;
    }

    void Material::SetFallbackShader() {
        params_float_.clear();
        params_vec4_.clear();

        if (!fallback_shader_program_) {
            fallback_shader_program_ = util::CreateProgram(tango_gl::shaders::kFallbackVS().c_str(),
                                                           tango_gl::shaders::kFallbackPS().c_str());
            if (!fallback_shader_program_) {
                LOGE("%s -- Critical error shader would not load.", __func__);
                abort();
            }
        }

        bool result = SetShaderInternal(fallback_shader_program_);
        if (!result) {
            LOGE(
                    "%s -- Critical error, could not get required uniforms and/or "
                            "attributes.",
                    __func__);
            abort();
        }
    }

    bool Material::SetShaderInternal(GLuint program) {
        shader_program_ = program;
        attrib_vertices_ = glGetAttribLocation(shader_program_, "vertex");
        if (attrib_vertices_ == -1) {
            LOGE("Could not get vertex attribute");
            // Positions are required.
            return false;
        }
        attrib_normals_ = glGetAttribLocation(shader_program_, "normal");
        attrib_colors_ = glGetAttribLocation(shader_program_, "color");
        attrib_uv_ = glGetAttribLocation(shader_program_, "uv");

        uniform_mvp_mat_ = glGetUniformLocation(shader_program_, "mvp");
        if (uniform_mvp_mat_ == -1) {
            LOGE("Could not get mvp uniform");
            // The Model-View-Projection matrix is required.
            return false;
        }

        uniform_mv_mat_ = glGetUniformLocation(shader_program_, "mv");
        uniform_m_mat_ = glGetUniformLocation(shader_program_, "m");
        uniform_normal_mat_ = glGetUniformLocation(shader_program_, "normal_mat");
        return true;
    }

    bool Material::SetParam(const char *uniform_name, float val) {
        if (shader_program_ == fallback_shader_program_) {
            // The fallback shader ignores all parameters to avoid cluttering
            // up the log.
            return true;
        }

        GLint location = glGetUniformLocation(shader_program_, uniform_name);
        if (location == -1) {
            LOGE("%s -- Unable to find parameter %s", __func__, uniform_name);
            return false;
        }

        params_float_[location] = val;
        return true;
    }

    bool Material::SetParam(const char *uniform_name, const glm::vec4 &vals) {
        if (shader_program_ == fallback_shader_program_) {
            // The fallback shader ignores all parameters to avoid cluttering
            // up the log.
            return true;
        }

        GLint location = glGetUniformLocation(shader_program_, uniform_name);
        if (location == -1) {
            LOGE("%s -- Unable to find parameter %s", __func__, uniform_name);
            return false;
        }

        params_vec4_[location] = vals;
        return true;
    }

    bool Material::SetParam(const char *uniform_name, Texture *texture) {
        if (shader_program_ == fallback_shader_program_) {
            // The fallback shader ignores all parameters to avoid cluttering
            // up the log.
            return true;
        }

        GLint location = glGetUniformLocation(shader_program_, uniform_name);
        if (location == -1) {
            LOGE("%s -- Unable to find parameter %s", __func__, uniform_name);
            return false;
        }

        params_texture_[location] = texture;
        return true;
    }

    void Material::BindParams() const {
        for (auto &param : params_float_) {
            glUniform1f(param.first, param.second);
        }
        for (auto &param : params_vec4_) {
            glUniform4fv(param.first, 1, glm::value_ptr(param.second));
        }
        int index = 0;
        for (auto &param : params_texture_) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(param.second->GetTextureTarget(),
                          param.second->GetTextureID());
            glUniform1i(param.first, index);
            index++;
        }
    }

} // tango_gl namespace