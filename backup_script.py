import git
import os

def backup_github_repo(repo_url, local_path):
    repo = git.Repo.init(local_path)

    origin = repo.create_remote('origin', repo_url)
    origin.fetch()

    remote_branches = origin.refs

    remote_branch_names = ['/'.join(branch.name.split('/')[1:]) for branch in remote_branches if branch.name.startswith('origin/')]

    for branch_name in remote_branch_names:
        branch_dir = '/'.join([local_path, branch_name])
        os.makedirs(branch_dir)
        git.Repo.clone_from(repo_url, branch_dir, branch=branch_name, multi_options=['--no-single-branch'])

if __name__ == "__main__":
    # URL del repositorio de GitHub
    repo_url = "https://github.com/Mario-Carmona/Weaver.git"  # Reemplaza esto con la URL de tu repositorio
    # Directorio local donde deseas realizar el backup
    local_path = "/home/macarse/Escritorio/EEZA/backup"  # Reemplaza esto con la ruta a tu directorio local

    # Verificar si el directorio local existe, si no, crearlo
    if not os.path.exists(local_path):
        os.makedirs(local_path)

    # Realizar el backup del repositorio
    backup_github_repo(repo_url, local_path)