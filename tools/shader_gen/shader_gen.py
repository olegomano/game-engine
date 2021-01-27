import os
import argparse 
import gles20_gen

class Shader:
    def __init__(self,type,path):
        self._type = type
        self._path = path 
        self._name = os.path.splitext(path)[0]
        self._id   = self._name.split("/")[-1]
class Scanner:
    def __init__(self,args):
        self._params = args
        self._shaders = []
        self._matches = {}

    def scan(self):
        for f in self._params.input:
            self.scan_dir_for_shaders(os.getcwd() + os.sep + f)
        for s in self._shaders:
            print("Found Shader: " + str(s._name))
            if s._name not in self._matches:
                self._matches[s._name] = []
            self._matches[s._name].append(s)

    def scan_dir_for_shaders(self,d):
        print("Scanning " + str(d))
        for dirName,subdirList,fileList in os.walk(d):
            for f in fileList:
                file_path = d + os.sep + f
                if file_path.endswith(".vert"):
                    self._shaders.append( Shader("Vertex",file_path) )
                if file_path.endswith(".frag"):
                    self._shaders.append( Shader("Fragment",file_path) )
            for subd in subdirList:
                self.scan_dir_for_shaders(d + os.sep + subd)
         

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('-i','--input', nargs='+', help='<Required> Shader Files', required=True, type=str) 
    parser.add_argument('-v','--verbose', nargs='+', help='<Optional> Verbose Printing', required=False, type=bool) 
    args = parser.parse_args()

    s = Scanner(args)
    s.scan()
    for shader in s._matches:
        try:
            g = gles20_gen.GLES20Generator(s._matches[shader], str(shader.split("/")[-1]) ) 
            g.generate(shader + ".h")
        except Exception as e:
            print(e)
