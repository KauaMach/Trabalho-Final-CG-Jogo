import numpy as np

def converter_obj_para_txt(caminho_obj, nome_arquivo_saida="funcao_virus.txt", nome_funcao="DesenharVirus1"):
    vertices = []
    faces = []

    try:
        with open(caminho_obj, 'r') as f:
            for linha in f:
                partes = linha.split()
                if not partes: continue
                if partes[0] == 'v':
                    vertices.append([float(partes[1]), float(partes[2]), float(partes[3])])
                elif partes[0] == 'f':
                    # Extrai índices
                    idx_face = [int(p.split('/')[0]) - 1 for p in partes[1:]]
                    faces.append(idx_face)
    except FileNotFoundError:
        print(f"Erro: O arquivo '{caminho_obj}' não foi encontrado.")
        return

    with open(nome_arquivo_saida, 'w') as f_saida:
        f_saida.write(f"#include \"../bibliotecas/renderer.h\"\n\n")
        f_saida.write(f"void Renderer::{nome_funcao}(float tamanho) {{\n")
        f_saida.write("    float s = tamanho;\n")
        f_saida.write("    glBegin(GL_TRIANGLES);\n")
        
        for face in faces:
            for i in range(1, len(face) - 1):
                v1 = np.array(vertices[face[0]])
                v2 = np.array(vertices[face[i]])
                v3 = np.array(vertices[face[i+1]])
                
                # --- CÁLCULO DA NORMAL ---
                edge1 = v2 - v1
                edge2 = v3 - v1
                normal = np.cross(edge1, edge2)
                
                # Normalização (essencial para a iluminação ser consistente)
                norm = np.linalg.norm(normal)
                if norm > 0:
                    normal = normal / norm
                
                # Escreve a normal calculada antes dos vértices
                f_saida.write(f"        glNormal3f({normal[0]:.4f}f, {normal[1]:.4f}f, {normal[2]:.4f}f);\n")
                
                # Escreve os vértices
                f_saida.write(f"        glVertex3f({v1[0]:.4f}f * s, {v1[1]:.4f}f * s, {v1[2]:.4f}f * s);\n")
                f_saida.write(f"        glVertex3f({v2[0]:.4f}f * s, {v2[1]:.4f}f * s, {v2[2]:.4f}f * s);\n")
                f_saida.write(f"        glVertex3f({v3[0]:.4f}f * s, {v3[1]:.4f}f * s, {v3[2]:.4f}f * s);\n")
                
        f_saida.write("    glEnd();\n")
        f_saida.write("}\n")

    print(f"Sucesso! Arquivo '{nome_arquivo_saida}' gerado com normais.")

# === COMO EXECUTAR ===
converter_obj_para_txt("virus1.obj", "renderer_virus1.cpp", "DesenharVirus1")
converter_obj_para_txt("virus2.obj", "renderer_virus2.cpp", "DesenharVirus2")
converter_obj_para_txt("virus3.obj", "renderer_virus3.cpp", "DesenharVirus3")
converter_obj_para_txt("virus4.obj", "renderer_virus4.cpp", "DesenharVirus4")
converter_obj_para_txt("atirador.obj", "renderer_player.cpp", "DesenharLeukocito")