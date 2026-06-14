def converter_obj_para_txt(caminho_obj, nome_arquivo_saida="funcao_virus.txt", nome_funcao="DesenharVirus1"):
    vertices = []
    faces = []

    # 1. Lê as linhas textuais do arquivo .obj
    try:
        with open(caminho_obj, 'r') as f:
            for linha in f:
                partes = linha.split()
                if not partes:
                    continue
                if partes[0] == 'v':
                    # Armazena as coordenadas x, y, z do vértice
                    vertices.append([float(partes[1]), float(partes[2]), float(partes[3])])
                elif partes[0] == 'f':
                    # Extrai apenas os índices dos vértices das faces (tratando o formato v/vt/vn)
                    idx_face = [int(p.split('/')[0]) - 1 for p in partes[1:]]
                    faces.append(idx_face)
    except FileNotFoundError:
        print(f"Erro: O arquivo '{caminho_obj}' não foi encontrado. Coloque-o na mesma pasta do script.")
        return

    # 2. Abre o arquivo .txt para gravar a função formatada em C++
    with open(nome_arquivo_saida, 'w') as f_saida:
        f_saida.write(f"#include \"../bibliotecas/renderer.h\"\n\nvoid Renderer::{nome_funcao}(float tamanho) {{\n")
        f_saida.write("    float s = tamanho;\n")
        f_saida.write("    glBegin(GL_TRIANGLES);\n")
        
        for face in faces:
            # Divide polígonos maiores (como quadriláteros) em triângulos simples
            for i in range(1, len(face) - 1):
                v1 = vertices[face[0]]
                v2 = vertices[face[i]]
                v3 = vertices[face[i+1]]
                
                # Grava os vértices multiplicados pelo fator de escala s no .txt
                f_saida.write(f"        glVertex3f({v1[0]:.4f}f * s, {v1[1]:.4f}f * s, {v1[2]:.4f}f * s);\n")
                f_saida.write(f"        glVertex3f({v2[0]:.4f}f * s, {v2[1]:.4f}f * s, {v2[2]:.4f}f * s);\n")
                f_saida.write(f"        glVertex3f({v3[0]:.4f}f * s, {v3[1]:.4f}f * s, {v3[2]:.4f}f * s);\n")
                
        f_saida.write("    glEnd();\n")
        f_saida.write("}\n")

    print(f"Sucesso! A funcao C++ foi gerada e salva no arquivo: '{nome_arquivo_saida}'")

# === COMO EXECUTAR ===

# Altere aqui o nome do seu arquivo vírus de entrada!
converter_obj_para_txt("virus1.obj", "renderer_virus1.cpp", "DesenharVirus1")
converter_obj_para_txt("virus2.obj", "renderer_virus2.cpp", "DesenharVirus2")
converter_obj_para_txt("virus3.obj", "renderer_virus3.cpp", "DesenharVirus3")
converter_obj_para_txt("virus4.obj", "renderer_virus4.cpp", "DesenharVirus4")
converter_obj_para_txt("atirador.obj", "renderer_player.cpp", "DesenharLeukocito")

