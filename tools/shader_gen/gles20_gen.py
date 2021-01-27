import os
import argparse 
import codecs 

class Uniform:
    def __init__(self,t,n):
        self._name = n
        self._type = t

class Attribute:
    def __init__(self,t,n):
        self._name = t
        self._type = n

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
        uf,af = self.extract_vars(fragment)
        uv,av = self.extract_vars(vertex)

        self._uniforms = self._uniforms + uf + uv
        self._attributes = self._attributes + af + av
 
        template = template.replace("{vertex_shader}",self.gen_c_array("vertex_shader",vertex ) )
        template = template.replace("{fragment_shader}",self.gen_c_array("fragment_shader",fragment ) )       
        template = template.replace("{shader_name}",self._name)
        template = template.replace("{members}",self.gen_members() ) 
        template = template.replace("{uniform_accessors}",self.gen_uniform_setters() )
        template = template.replace("{attribute_accessors}","" )
        template = template.replace("{bind_body}",self.gen_bind())
        
        out = open(targetFile,'w')
        out.write(template)
        out.close()

    def extract_vars(self,shader):
        uniforms = []
        attribs = []
        for line in shader.splitlines():
            tokens = line.split(" ")
            if tokens[0] == "uniform":
                t = tokens[1]
                n = tokens[2][0:-1] #remove ; form end
                uniforms.append(Uniform(t,n))
            if tokens[0] == "in":
                t = tokens[1][0:-1]
                n = tokens[2]
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
        return result 


    def gen_uniform_setters(self):
        result = ""
        for uniform in self._uniforms:
            result = result + "void setUniform_" + str(uniform._name) + "("
            if uniform._type == "mat4":
                result += "const float* const matrix){"
                result += "glUniformMatrix4fv(" + uniform._name + ", 1, GL_FALSE, matrix);}\n\t"    
            elif uniform._type == "vec4":
                result += "const float* const vector){"
                result += "glUniform4f(" + uniform._name + ",vector[0],vector[1],vector[2],vector[3]);}\n\t"
            else:
                result += "){static_assert(false, \"unsupported type\"); }\n\t"
        return result;

    def gen_attribute(self,attrib):
        pass

    def gen_c_array(self,name,buff):
        result = "static constexpr char " + str(name) + "[" + str(len(buff) + 1) + "] = {"
        for i in buff:
            result += str(hex(ord(i))) + ","
        result += "0x00"
        result += "};"
        return result

