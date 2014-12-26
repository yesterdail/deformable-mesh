#include "GLShader.h"
#include "GLUtility.h"
#include <cassert>
#include <iostream>

namespace hj
{
  GLShader::GLShader(GLenum type) 
    : id(0), type(type) 
  {
    if(type == GL_GEOMETRY_SHADER) {
      assert(GL_GEOMETRY_SHADER);
    }
    if (type == GL_COMPUTE_SHADER) {
      assert(GL_COMPUTE_SHADER);
    }
    id = glCreateShader(type);
  }

  GLShader::~GLShader() 
  {
    glDeleteShader(id);
  }

  std::string GLShader::GetSource() const 
  {
    return source;
  }

  bool GLShader::SetSource(const std::string &source)
  {
    this->source = source;

    // Need a char** for glShaderSource
    const char *s = source.c_str();
    glShaderSource(id, 1, &s, 0);
    // Compile the shader
    glCompileShader(id);
    return IsOk();
  }

  bool GLShader::IsOk() const 
  {
    GLint ok = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    return (ok == GL_TRUE);
  }

  std::string GLShader::GetInfoLog() const 
  {
    int loglength = 0;

    // Get the length of the log
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &loglength);
    if (loglength > 0) 
    {
      // Allocate a buffer and get the log
      int dummy;
      char *buffer = new char[loglength];
      glGetShaderInfoLog(id, loglength, &dummy, buffer);
      std::string infolog(buffer);
      delete[] buffer;

      return infolog;
    } 
    else 
    {
      // No log...
      return std::string("");
    }
  }

  struct GLProgram::GLAttributeInfo
  {
    int size;
    unsigned int type;
    std::string name;
    int index;
  };

  struct GLProgram::GLUniformInfo
  {
    int size;
    unsigned int type; 
    std::string name;
  };

  GLProgram::GLProgram() 
    : inuse(false) 
  {
    assert(GLEW_VERSION_2_0);
    id = glCreateProgram();
  }

  GLProgram::~GLProgram() 
  {
    for (std::set<GLShader*>::iterator i = shaders.begin(); i != shaders.end();) 
    {
      // Grab shader reference
      GLShader *shader = *i;
      // Increase iterator to prevent invalidating it
      ++i;
      // Detach shader
      DetachShader(*shader);
      // Delete shader
      delete shader;
    }
    shaders.clear();

    // Delete the program
    glDeleteProgram(id);
  }

  bool GLProgram::AddVertexShader(const std::string &source) 
  {
    GLShader *shader = new GLShader(GL_VERTEX_SHADER);
    if (!shader) return false;

    if (!shader->SetSource(source)) 
    {
      std::cerr << "Program: Error compiling vertex shader, dumping log..." << std::endl;
      std::cerr << shader->GetInfoLog() << std::endl;
      std::cerr << "Program: Vertex shader source:" << std::endl;
      std::cerr << shader->GetSource() << std::endl;
      assert(0);
      return false;
    }

    AttachShader(*shader);
    return true;
  }

  bool GLProgram::AddVertexShaderFile(const std::string &filename)
  {
    std::string source = GLUtility::GetShaderSource(filename);
    return this->AddVertexShader(source);
  }

  bool GLProgram::AddGeometryShader(const std::string &source)
  {
    GLShader *shader = new GLShader(GL_GEOMETRY_SHADER);
    if (!shader) return false;

    if (!shader->SetSource(source))
    {
      std::cerr << "Program: Error compiling geometry shader, dumping log..." << std::endl;
      std::cerr << shader->GetInfoLog() << std::endl;
      std::cerr << "Program: Geometry shader source:" << std::endl;
      std::cerr << shader->GetSource() << std::endl;
      assert(0);
      return false;
    }

    AttachShader(*shader);
    return true;
  }

  bool GLProgram::AddGeometryShaderFile(const std::string &filename)
  {
    std::string source = GLUtility::GetShaderSource(filename);
    return this->AddGeometryShader(source);
  }

  bool GLProgram::AddFragmentShader(const std::string &source) 
  {
    GLShader *shader = new GLShader(GL_FRAGMENT_SHADER);
    if (!shader) return false;

    if (!shader->SetSource(source)) 
    {
      std::cerr << "Program: Error compiling fragment shader, dumping log..." << std::endl;
      std::cerr << shader->GetInfoLog() << std::endl;
      std::cerr << "Program: Fragment shader source:" << std::endl;
      std::cerr << shader->GetSource() << std::endl;
      assert(0);
      return false;
    }

    AttachShader(*shader);
    return true;
  }

  bool GLProgram::AddFragmentShaderFile(const std::string &filename)
  {
    std::string source = GLUtility::GetShaderSource(filename);
    return this->AddFragmentShader(source);
  }

  bool GLProgram::AddComputeShader(const std::string &source)
  {
    GLShader *shader = new GLShader(GL_COMPUTE_SHADER);
    if (!shader) return false;

    if (!shader->SetSource(source)) 
    {
      std::cerr << "Program: Error compiling compute shader, dumping log..." << std::endl;
      std::cerr << shader->GetInfoLog() << std::endl;
      std::cerr << "Program: Compute shader source:" << std::endl;
      std::cerr << shader->GetSource() << std::endl;
      assert(0);
      return false;
    }

    AttachShader(*shader);
    return true;
  }

  bool GLProgram::AddComputeShaderFile(const std::string &filename)
  {
    std::string source = GLUtility::GetShaderSource(filename);
    return this->AddComputeShader(source);
  }

  void GLProgram::SetParameteri(int param, int value)
  {
    assert(GL_GEOMETRY_SHADER);
    glProgramParameteriEXT(id, param, value);
  }

  void GLProgram::AttachShader(GLShader *shader) 
  {
    if (shader) AttachShader(*shader);
  }

  void GLProgram::AttachShader(GLShader &shader) 
  {
    glAttachShader(id, shader.id);
    shaders.insert(&shader);
  }

  void GLProgram::DetachShader(GLShader *shader) 
  {
    if (shader) DetachShader(*shader);
  }

  void GLProgram::DetachShader(GLShader &shader) 
  {
    glDetachShader(id, shader.id);
    shaders.erase(&shader);
  }

  bool GLProgram::Link() 
  {
    glLinkProgram(id);
    bool ok = IsOk();
    if (!ok) 
    {
      std::cerr << "Program: Error linking shader program, dumping log..." << std::endl;
      std::cerr << GetInfoLog() << std::endl;
      assert(0);
    }
    return ok;
  }

  bool GLProgram::IsOk() const 
  {
    GLint ok = GL_FALSE;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    return (ok == GL_TRUE);
  }

  std::string GLProgram::GetInfoLog() const 
  {
    int loglength = 0;

    // Get the length of the log
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglength);
    if (loglength > 0) 
    {
      // Allocate a buffer and get the log
      int dummy;
      char *buffer = new char[loglength];
      glGetProgramInfoLog(id, loglength, &dummy, buffer);
      std::string infolog(buffer);
      delete[] buffer;

      return infolog;
    } 
    else 
    {
      // No log...
      return std::string("");
    }
  }

  std::string GLProgram::GetInfoLogs() const 
  {
    std::string log = "===Shader logs===\r\n";
    // Get shader logs
    for (std::set<GLShader*>::const_iterator i = shaders.begin(); 
      i != shaders.end(); ++i) 
    {
      log += (*i)->GetInfoLog();
      log += "\r\n-----------------\r\n";
    }
    // Get program log
    log += "===Program log===\r\n";
    log += GetInfoLog();

    return log;
  }

  void GLProgram::Bind() 
  {
    assert(!inuse);
    if (inuse) return;

    glUseProgram(id);
    inuse = true;
  }

  void GLProgram::Unbind() 
  {
    assert(inuse);
    if (!inuse) return;

    glUseProgram(0);
    inuse = false;
  }

  bool GLProgram::SetUniform1f(const std::string &name, float v0) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform1f(loc, v0);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform2f(const std::string &name, float v0, float v1) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform2f(loc, v0, v1);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform3f(const std::string &name, float v0, float v1, float v2) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform3f(loc, v0, v1, v2);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform4f(const std::string &name, 
    float v0, float v1, float v2, float v3) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform4f(loc, v0, v1, v2, v3);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform1fv(const std::string &name, int num, const float *v) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform1fv(loc, num, v);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform2fv(const std::string &name, int num, const float *v) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform2fv(loc, num, v);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform3fv(const std::string &name, int num, const float *v) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform3fv(loc, num, v);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform4fv(const std::string &name, int num, const float *v) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform4fv(loc, num, v);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform1i(const std::string &name, int v0) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform1i(loc, v0);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform2i(const std::string &name, int v0, int v1) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform2i(loc, v0, v1);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform3i(const std::string &name, int v0, int v1, int v2) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform3i(loc, v0, v1, v2);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniform4i(const std::string &name, 
    int v0, int v1, int v2, int v3) 
  { 
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform4i(loc, v0, v1, v2, v3);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniformMatrix2fv(const std::string &name, 
    int count, float *v, bool transpose)
  {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniformMatrix2fv(loc, count, (transpose ? 1 : 0), v);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniformMatrix3fv(const std::string &name, 
    int count, float *v, bool transpose)
  {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniformMatrix3fv(loc, count, (transpose ? 1 : 0), v);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::SetUniformMatrix4fv(const std::string &name, 
    int count, float *v, bool transpose)
  {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniformMatrix4fv(loc, count, (transpose ? 1 : 0), v);
    return true;
  }

  // ----------------------------------------------------------------------------
  bool GLProgram::UseTexture(const std::string &name, int texunit) 
  {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1) {
      assert(0);
      return false;
    }

    glUniform1i(loc, texunit);
    return true;
  }

  // ----------------------------------------------------------------------------
  // The program needs to be re-linked for the change to take effect
  // Matrices are automatically bound as column vectors to index + i, i = 0..n
  bool GLProgram::BindAttribLocation(const std::string &name, int index)
  {
    int maxAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    if (index < 0 || index >= maxAttribs)
      return false;

    glBindAttribLocation(id, index, name.c_str());
    return true;
  }

  // ----------------------------------------------------------------------------
  // Returns the assigned location of the given attribute, or -1 if not found
  // The program needs to be linked for this to work
  int GLProgram::GetAttribLocation(const std::string &name)
  {
    int loc = glGetAttribLocation(id, name.c_str());
    return loc;
  }

  // ----------------------------------------------------------------------------
  // Returns information on the active attributes in the current program
  std::vector<GLProgram::GLAttributeInfo> GLProgram::GetActiveAttributes()
  {
    std::vector<GLAttributeInfo> attribs;

    // Check if program is OK first
    if (!IsOk())
    {
      std::cerr << "Program: Program not ok, can't enumerate attributes..." 
        << std::endl;
      assert(0);
      return attribs;
    }

    // Get number of active attributes and max attribute name length
    int numAttribs;
    glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &numAttribs);
    int maxAttribNameLength;
    glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);

    // Temporary buffer for attribute names
    char *name = new char[maxAttribNameLength];
    unsigned int type = GL_NONE;
    int size = 0;
    int length = 0;

    // Enumerate attributes
    for (int i = 0; i < numAttribs; ++i)
    {
      // Get attrib info
      glGetActiveAttrib(id, i, maxAttribNameLength, 
        &length, &size, &type, name);
      // Add to list
      GLAttributeInfo attrib;
      attrib.size = size;
      attrib.type = type;
      attrib.name = std::string(name);
      attrib.index = GetAttribLocation(attrib.name);
      attribs.push_back(attrib);
    }

    // Clean up and return
    delete [] name;

    return attribs;
  }

  // ----------------------------------------------------------------------------
  // Returns information on the active uniforms in the current program
  std::vector<GLProgram::GLUniformInfo> GLProgram::GetActiveUniforms()
  {
    std::vector<GLUniformInfo> uniforms;

    // Check if program is OK first
    if (!IsOk())
    {
      std::cerr << "Program: Program not ok, can't enumerate uniforms..." 
        << std::endl;
      assert(0);
      return uniforms;
    }

    // Get number of active uniforms and max uniform name length
    int numUniforms;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numUniforms);
    int maxUniformNameLength;
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);

    // Temporary buffer for uniform names
    char *name = new char[maxUniformNameLength];
    unsigned int type = GL_NONE;
    int size = 0;
    int length = 0;

    // Enumerate uniforms
    for (int i = 0; i < numUniforms; ++i)
    {
      // Get uniform info
      glGetActiveUniform(id, i, maxUniformNameLength, 
        &length, &size, &type, name);
      // Add to list
      GLUniformInfo uniform;
      uniform.size = size;
      uniform.type = type;
      uniform.name = std::string(name);
      uniforms.push_back(uniform);
    }

    // Clean up and return
    delete [] name;

    return uniforms;
  }
}