#include "../include/shader.h"
#include <iostream>
#include <fstream>

Shader::Shader(const std::string& fileName)
{
	m_program = glCreateProgram();
	m_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
	m_shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);

	for(unsigned int i = 0; i < NUM_SHADERS; i++)
		glAttachShader(m_program, m_shaders[i]);

	glBindAttribLocation(m_program, 0, "position");
	glBindAttribLocation(m_program, 1, "texCoord");
	glBindAttribLocation(m_program, 2, "normal");

	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Invalid shader program");

	m_uniforms["MVP"] = glGetUniformLocation(m_program, "MVP");
	m_uniforms["Normal"] = glGetUniformLocation(m_program, "Normal");
	m_uniforms["MaterialAmbientColor"] = glGetUniformLocation(m_program, "MaterialAmbientColor");
	m_uniforms["eyePos"] = glGetUniformLocation(m_program , "eyePos");
	m_uniforms["specularPower"] = glGetUniformLocation(m_program , "specularPower");
	m_uniforms["specularIntensity"] = glGetUniformLocation(m_program , "specularIntensity");

    for(int i = 0 ; i< NUM_POINT_LIGHTS ; i++)
    {
        std::string light = "pointLights[" + std::to_string(i) + "]";
        m_uniforms[light + ".base.color"] = glGetUniformLocation(m_program ,(light+".base.color").c_str());
        m_uniforms[light + ".base.intensity"] = glGetUniformLocation(m_program ,(light+".base.intensity").c_str());
        m_uniforms[light + ".position"] = glGetUniformLocation(m_program ,(light+".position").c_str());
        m_uniforms[light + ".atten.constant"] = glGetUniformLocation(m_program ,(light+".atten.constant").c_str());
        m_uniforms[light + ".atten.linear"] = glGetUniformLocation(m_program ,(light+".atten.linear").c_str());
        m_uniforms[light + ".atten.exponent"] = glGetUniformLocation(m_program ,(light+".atten.exponent").c_str());
        m_uniforms[light + ".range"] = glGetUniformLocation(m_program ,(light+".range").c_str());
    }
}

Shader::~Shader()
{
	for(unsigned int i = 0; i < NUM_SHADERS; i++)
    {
        glDetachShader(m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }

	glDeleteProgram(m_program);
}

void Shader::Bind()
{
	glUseProgram(m_program);
}

void Shader::Update(const Transform& transform, const Camera& camera)
{
	glm::mat4 MVP = transform.GetMVP(camera);
	glm::mat4 Normal = transform.GetModel();

	setUniformMatrix4f("MVP" , &MVP[0][0]);
	setUniformMatrix4f("Normal" , &Normal[0][0]);

	setUniform1f("specularPower" , 5);
	setUniform1f("specularIntensity" , 8);

    PointLight p1[2] = { PointLight(glm::vec3(1.0f , 0.0f , 0.0f) , 5.0f ,
	glm::vec3(0.0f , 6.0f , 2.0f)) ,  PointLight(glm::vec3(0.0f , 1.0f , 0.0f) , 5.0f ,
	glm::vec3(0.0f , 6.0f , 2.0f))};


    setUniformPointLights(p1 , 2);

	setUniformVector4f("MaterialAmbientColor" , 0.1f , 0.1f , 0.1f , 1.0f);
}
void Shader::setUniformPointLights(PointLight* pArray , int n)
{
    for(int i = 0 ;i< n ; i++)
    {
        std::string light = "pointLights[" + std::to_string(i) + "]";
        glm::vec3 color = pArray[i].getColor();
        glm::vec3 pos = pArray[i].getPosition();
        float intensity = pArray[i].getIntensity();
        float range = pArray[i].getRange();
        Attenuation att = pArray[i].getAttenuation();

        setUniformVector3f(light+".base.color" ,color.x , color.y , color.z);
        setUniform1f(light+".base.intensity" , intensity);
        setUniform1f(light+".range" , range);
        setUniform1f(light+".atten.constant" , att.getConstant());
        setUniform1f(light+".atten.linear" , att.getLinear());
        setUniform1f(light+".atten.exponent" , att.getExponent());
        setUniformVector3f(light + ".position" , pos.x , pos.y , pos.z);

    }
}

std::string Shader::LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
			output.append(line + "\n");
        }
    }
    else
    {
		std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if(isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        if(isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}

GLuint Shader::CreateShader(const std::string& text, unsigned int type)
{
    GLuint shader = glCreateShader(type);

    if(shader == 0)
		std::cerr << "Error compiling shader type " << type << std::endl;

    const GLchar* p[1];
    p[0] = text.c_str();
    GLint lengths[1];
    lengths[0] = text.length();

    glShaderSource(shader, 1, p, lengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

    return shader;
}