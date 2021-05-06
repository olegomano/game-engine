import os
import argparse 
import codecs 
from datetime import datetime

class Uniform:
    def __init__(self,t,n):
        self._name = n
        self._type = t

class Attribute:
    def __init__(self,t,n):
        self._name = n
        self._type = t

class GLES20Generator:
    #shaderFiles is a array of frag/vert/geometry
    def __init__(self,shaders,name):
        self._name = name
        self._shaders = {}
        self._uniforms = []
        self._attributes = []


        for s in shaders:
            self._shaders[s._type] = s

    def generate(self,targetFile):
        print("Generating " + str(targetFile))

        fragment = open(self._shaders["Fragment"]._path,'r').read()
        vertex =  open(self._shaders["Vertex"]._path,'r').read()
        template = open("./shader_template.txt",'r').read()
        uf,af = self.extract_vars(fragment,"fragment")
        uv,av = self.extract_vars(vertex,"vertex")

        self._uniforms = self._uniforms + uf + uv
        self._attributes = self._attributes + af + av
            
        print(self._uniforms)
        print(self._attributes)
        template = template.replace("{timestamp}",str(datetime.now()))        
        template = template.replace("{vertex_shader}",self.gen_c_array("vertex_shader",vertex ) )
        template = template.replace("{fragment_shader}",self.gen_c_array("fragment_shader",fragment ) )       
        template = template.replace("{shader_name}",self._name)
        template = template.replace("{members}",self.gen_members() ) 
        template = template.replace("{uniform_accessors}",self.gen_uniform_setters() )
        template = template.replace("{attribute_accessors}",self.gen_attribute_setters() )
        template = template.replace("{bind_body}",self.gen_bind())
        template = template.replace("{enable_vertex_array}",self.gen_enable_vertex_attrib())
        template = template.replace("{disable_vertex_array}",self.gen_disbale_vertex_attrib())
        
        out = open(targetFile,'w')
        out.write(template)
        out.close()

    def extract_vars(self,shader,type):
        uniforms = []
        attribs = []
        for line in shader.splitlines():
            tokens = line.split(" ")
            if tokens[0] == "uniform":
                t = tokens[1]
                n = tokens[2][0:-1] #remove ; form end
                uniforms.append(Uniform(t,n))
            if tokens[0] == "in" and type == "vertex":
                t = tokens[1]
                n = tokens[2][0:-1]
                attribs.append(Attribute(t,n))
        return uniforms,attribs
   
    def gen_members(self):
        result = ""
        full_list = self._uniforms + self._attributes
        for i in full_list:
            result += "GLint " + i._name + ";\n\t"
        return result

    def gen_bind(self):
        result = ""
        for uniform in self._uniforms:
            result += uniform._name +  " = glGetUniformLocation(m_program,\"" + str(uniform._name) + "\");\n\t\t"
            result+="CheckGLError();\n\t\t"
        
        locationCount = 0
        for attr in self._attributes:
            result += "glBindAttribLocation(m_program," + str(locationCount) + ",\"" + str(attr._name) + "\");\n\t\t"
            result += str(attr._name) + "=" + str(locationCount) + ";\n\t\t"
            result += "CheckGLError();\n\t\t"
            locationCount += 1
        return result 


    def gen_uniform_setters(self):
        result = ""
        for uniform in self._uniforms:
            result = result + "void setUniform_" + str(uniform._name) + "("
            if uniform._type == "mat4":
                result += "const float* const matrix){"
                result += "glUniformMatrix4fv(" + uniform._name + ", 1, GL_FALSE, matrix);\n\t"    
            elif uniform._type == "vec4":
                result += "const float* const vector){"
                result += "glUniform4f(" + uniform._name + ",vector[0],vector[1],vector[2],vector[3]);\n\t"
            else:
                result += "){static_assert(false, \"unsupported type\"); }\n\t"
            result += "CheckGLError();}\n\t"
        return result

    def gen_attribute_setters(self):
        result = ""
        for a in self._attributes:
            result += "void setAttribute_" + str(a._name) + "(GLint buffer, uint32_t count){"
            
            result += "glBindBuffer(GL_ARRAY_BUFFER, buffer);"
            result += "glVertexAttribPointer(" + str(a._name) + ","
            if a._type == "vec4":
                result += "4"
            elif a._type == "vec3":
                result += "3"
            elif a._type == "vec2":
                result += "2"
            result += ",GL_FLOAT,GL_FALSE,0,NULL);" 
            result+="CheckGLError();}\n\t"
            
        return result

    def gen_enable_vertex_attrib(self):
        result= "" 
        for a in self._attributes:
            result += "glEnableVertexAttribArray(" + str(a._name) + ");\n\t\t"
        return result

    def gen_disbale_vertex_attrib(self):
        result= "" 
        for a in self._attributes:
            result += "glDisableVertexAttribArray(" + str(a._name) + ");\n\t\t"
        return result

    def gen_c_array(self,name,buff):
        result = "static constexpr char " + str(name) + "[" + str(len(buff) + 1) + "] = {"
        for i in buff:
            result += str(hex(ord(i))) + ","
        result += "0x00"
        result += "};"
        return result

