#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define LISTENING_PORT 5094
#define CONNECTION_HOST "127.0.0.1"
#define BUFFER_SIZE 1024


typedef struct contact {
	char nom[30],prenom[30],email[30],rue[30],ville[30],pays[30];
long long int  GSM;

}contact;

void afficher_contact(contact c){
	printf("\n\n\t   Nom : %s\n", c.nom);
    printf("\t   Prenom : %s\n", c.prenom);
    printf("\t   Email : %s\n", c.email);
    printf("\t   Adresse : %s %s %s\n", c.rue, c.ville, c.pays);
    printf("\t   GSM : %lld\n", c.GSM);
}

int menu_admin(){
	int choix;
	printf("\n\n\n\n\n\t\t*******************   MENU ADMIN   ********************\n\n");
    printf("\t\t      1. Ajouter un contact \n");
	printf("\t\t      2. Rechercher un contact\n");
    printf("\t\t      3. Supprimer un contact \n");
    printf("\t\t      4. Modifier un contact\n");
    printf("\t\t      5. Afficher tous les contacts\n");
    printf("\t\t      6. Quitter\n");
    printf("\n\t\t            Entrez votre choix :     ");
    scanf("%d", &choix);
    return choix;
}

int menu_invite(){
	int choix;
    printf("\n\n\n\n\n\t\t*******************   MENU INVITE   ********************\n\n");
    printf("\t\t      1. Rechercher un contact\n");
    printf("\t\t      2. Afficher tous les contacts\n");
    printf("\t\t      3. Quitter\n");
    printf("\n\t\t            Entrez votre choix :     ");
    scanf("%d", &choix);
    return choix;

}




contact saisir_contact(){
	contact c;
	printf("\n\n\n\t- Ajouter un contact :");
	printf("\n\n\n\t   Entrez le nom du contact :    ");
	scanf("%s", c.nom);
	printf("\t   Entrez le prenom du contact :    ");
	scanf("%s", c.prenom);
	printf("\t   Entrez le GSM :    ");
	scanf("%lld",&c.GSM);
	printf("\t   Entrez l'email :    ");
	scanf("%s", c.email);
	printf("\t   Entrez le nom de la rue :    ");
	scanf("%s", c.rue);
	printf("\t   Entrez la ville du contact :    ");
	scanf("%s", c.ville);
	printf("\t   Entrez le pays du contact :    ");
	scanf("%s", c.pays);
	printf("\n\t --> Contact saisi :     %s  %s  %s  %lld  %s  %s  %s\n",c.nom,c.prenom,c.email,c.GSM, c.rue,c.ville, c.pays);
	return c;
}





int main() {
	
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "(serveur) Echec d'initialisation de Winsock \n");
        exit(1);
    }
    //Creation du socket 
    int socketFD=socket(AF_INET,SOCK_STREAM,0);
    if (socketFD ==-1){
    	fprintf(stderr,"(client) Echec d'initialisation du socket \n");
    	exit(1);
	}
	// configuration et liason du socket
	struct sockaddr_in socketAddress;
	socketAddress.sin_family=AF_INET;
	socketAddress.sin_port=LISTENING_PORT;
	
	int inetReturnCode= inet_pton(AF_INET, CONNECTION_HOST, &socketAddress.sin_addr);
	if (inetReturnCode == -1){
		fprintf(stderr,"(client) adresse invalide ou non prise en charge \n");
		exit(1);
	}
	int socketAddressLength =sizeof(socketAddress);
	int connectionStatus=connect(socketFD,(struct sockaddr*) &socketAddress,socketAddressLength);
	if(connectionStatus ==-1){
		fprintf(stderr,"(client) Echec de la connexion au seveur \n");
		exit(1);
	}
	int quitter_serveur= 2;
	while(quitter_serveur==2){
	
	printf("\n\n\t***************************   BIENVENUE   ***************************   \n\n\n");
	printf("\tRQ: vous avez le droit d'entrer comme -ADMIN ou -INVITE seulement :) \n\n\n\n");
	int u_exist;

	do {
	
	char username[20];
	printf("   Veuillez saisir votre login :      ");
	scanf("%s", username);
	
	int sentBytes1=send(socketFD,username,strlen(username),0);

	int  receivedBytes1=recv(socketFD,&u_exist,sizeof(u_exist),0);

	} while (u_exist==0);
	
	int p_exist;
	int essaies=0;
	
	do {
		if (essaies<3) {
			
			char password[20];
			printf("\n   Veuillez saisir votre mot de passe :      ");
			scanf("%s", password);
			send(socketFD,password,strlen(password),0);
			essaies++;
			recv(socketFD,&p_exist,sizeof(p_exist),0);
    		}  
    	
    	else {
    		printf("\n\n\tNombre d'essais depasse. \n\n\n\n\n\n\t\tAu revoir :)\n\n\n\n\n\n\n");
    		exit(1);
		}
		
	} while(p_exist==0);
	
	// admin ou invite ;
	int admin;
	recv(socketFD,&admin,sizeof(admin),0);

	if(admin==0){
	
		printf("\n\n   --------------   VOUS ETES INVITE :)  --------------- ");

			int choix_invite;
			
			do {
				
				choix_invite = menu_invite();
				send(socketFD,&choix_invite,sizeof(choix_invite),0);
			
				int nombre_lignes_contact;
				
				switch (choix_invite) {
					
					case 1:
						printf("\n\n\n\t- Rechercher un contact :");
						printf("\n\n\t   Le nom de l'utilisateur a rechercher :     ");
						char nom_a_rechercher[20];
						scanf(" %s", nom_a_rechercher);
						send(socketFD,nom_a_rechercher,strlen(nom_a_rechercher),0);
						
						printf("\t   Le prenom de l'utilisateur a rechercher :     ");
						char prenom_a_rechercher[20];
						scanf(" %s", prenom_a_rechercher);
						send(socketFD,prenom_a_rechercher,strlen(prenom_a_rechercher),0);
					
						contact resultat;
						recv(socketFD,&resultat,sizeof(resultat),0);
					
						if(strcmp(resultat.nom, "")==0){
							printf("\n\t-->  Contact introuvable !! \n");
						}
						else {
							printf("\n\t-->  Contact trouve :   %s %s  %s  %s  %s  %s  %lld \n", resultat.nom, resultat.prenom,resultat.email,resultat.pays,resultat.ville,resultat.rue,resultat.GSM);
						}
						break;
						
					case 2:
						recv(socketFD,&nombre_lignes_contact,sizeof(nombre_lignes_contact),0);
						
						printf("\n\n\n\t- Liste des contacts :    %d", nombre_lignes_contact);
						int i=0;
						contact c2;
						
						while(i<nombre_lignes_contact){
    						recv(socketFD,&c2,sizeof(c2),0);
    						afficher_contact(c2);
    						i++;
						}
						break;
						
					case 3: 
						printf("\n\n\n\n\n\n\t\tAu revoir :)\n\n\n\n\n\n\n");
						break;
				}
			} while (choix_invite!=3);
	}
	
	
	
	

	if(admin==1){
		
		printf("\n\n   --------------   VOUS ETES ADMIN :)  --------------- ");
		
			int choix_admin;
			
			do {
				choix_admin = menu_admin();
				send(socketFD,&choix_admin,sizeof(choix_admin),0);
				
				int nombre_lignes_contact;
				contact c3;
				
				switch (choix_admin) {
					
					case 2:
						printf("\n\n\n\t- Rechercher un contact :");
						printf("\n\n\t   Le nom de l'utilisateur a rechercher :     ");
						char nom_a_rechercher[20];
						scanf(" %s", nom_a_rechercher);
						send(socketFD,nom_a_rechercher,strlen(nom_a_rechercher),0);
					
						printf("\t   Le prenom de l'utilisateur a rechercher :     ");
						char prenom_a_rechercher[20];
						scanf(" %s", prenom_a_rechercher);
						send(socketFD,prenom_a_rechercher,strlen(prenom_a_rechercher),0);
					
						contact resultat;
						recv(socketFD,&resultat,sizeof(resultat),0);
						
						if(strcmp(resultat.nom, "")==0) {
							printf("\n\t-->  Contact introuvable !! \n");
						}
						else {
							printf("\n\t-->  Contact trouve :   %s %s  %s  %s  %s  %s  %lld \n", resultat.nom, resultat.prenom,resultat.email,resultat.ville,resultat.pays,resultat.rue,resultat.GSM);
						}
						break;
						
					case 5:
						
						recv(socketFD,&nombre_lignes_contact,sizeof(nombre_lignes_contact),0);
						printf("\n\n\n\t- Liste des contacts :    %d", nombre_lignes_contact);
						int i=0;
						contact c2;
						
						while(i<nombre_lignes_contact){
    						recv(socketFD,&c2,sizeof(c2),0);
    						afficher_contact(c2);
    						i++;
						}
						break;
						
					case 1:
						
						c3=saisir_contact();
						send(socketFD,&c3,sizeof(c3),0);
						break;
						
					case 3:
						
						printf("");
						int suppression;
						printf("\n\n\n\t- Supprimer un contact :");
                    	printf("\n\n\t   Le nom de l'utilisateur a Supprimer :    ");
						char nom_a_supprimer[20];
						scanf("%s", nom_a_supprimer);
						send(socketFD,nom_a_supprimer,20,0);
						
						printf("\t   Le prenom de l'utilisateur a supprimer :    ");
						char prenom_a_supprimer[20];
						scanf("%s", prenom_a_supprimer);
						send(socketFD,prenom_a_supprimer,20,0);
						
						recv(socketFD, &suppression , sizeof(suppression), 0);
                        
                        if (suppression== 0) {
                            printf("\n\t-->  Echec de la suppression du contact !\n");
                        }
                        else{
                        	printf("\n\t-->  Contact Supprime avec success !\n");
                        }
						break;
					case 4:
						
						printf("");
						printf("\n\n\n\t- Modifier un contact un contact :");
                    	printf("\n\n\t   Le nom de l'utilisateur a Modifier :    ");
						char nom_a_modifier[20];
						scanf("%s", nom_a_modifier);
						send(socketFD,nom_a_modifier,20,0);
						
						printf("\t   Le prenom de l'utilisateur a modifier :    ");
						char prenom_a_modifier[20];
						scanf("%s", prenom_a_modifier);
						send(socketFD,prenom_a_modifier,20,0);
						contact resultat1;
                        
                        recv(socketFD,&resultat1,sizeof(resultat1),0);
						
						if(strcmp(resultat1.nom, "")==0) {
							printf("\n\n\t-->  Le contact que vous voulez modifier est introuvable !! \n");
						}
						else {
							printf("\n\t-->  Contact a modifier :   %s  %s %s  %s  %s  %s  %lld \n", resultat1.nom, resultat1.prenom,resultat1.email,resultat1.pays,resultat1.ville,resultat1.rue,resultat1.GSM);
							int choice_modify;

							do{
							
								printf("\n\t   Modifier :  \n");
            					printf("\n\t       1. Nom");
            					printf("\n\t       2. Prenom");
            					printf("\n\t       3. Email");
            					printf("\n\t       4. GSM");
            					printf("\n\t       5. Adresse");
            					printf("\n\t       6. Finir les modifications \n");
								printf("\n\t   Entrer votre choix :  ");
            					//int choice_modify;
            					scanf("%d", &choice_modify);
            					switch (choice_modify) {
            					case 1:
                    				printf("\n\t       Entrer le nouveau nom : ");
                    				char new_nom[40];
                    				scanf("%s", new_nom);
                    				strcpy(resultat1.nom,new_nom );

                    			break;
            					case 2:
                    				printf("\n\t       Entrer le nouveau prenom : ");
                    				char new_prenom[40];
                    				scanf("%s", new_prenom);
                    				strcpy(resultat1.prenom,new_prenom );

                    			break;
                    			case 3:
                    				printf("\n\t       Entrer le nouveau email : ");
                    				char new_mail[40];
                    				scanf("%s", new_mail);
                    				strcpy(resultat1.email,new_mail );

                    			break;
                    			case 4:
                    				printf("\n\t       Entrer le nouveau GSM : ");
                    				long long int new_GSM;
                    				scanf("%lld", &new_GSM);
                    				resultat1.GSM=new_GSM;

                    			break;
                    			case 5:
									printf("\n\t       Entrer la nouvelle rue :    ");
									char new_rue[40];
									scanf("%s", new_rue);
									strcpy(resultat1.rue,new_rue);
									printf("\t       Entrer la nouvelle ville :    ");
									char new_ville[40];
									scanf("%s", new_ville);
									strcpy(resultat1.ville,new_ville);
									printf("\t       Entrer le nouveau pays :    ");
									char new_pays[40];
									scanf("%s", new_pays);
									strcpy(resultat1.pays,new_pays);

								break;
								case 6:
								printf("\n\t-->  Les modifications appliquees avec succes !\n");
								break;
								
					}
					
					if (choice_modify != 6 ) { 
						printf("\n\t-->  Contact modifie' avec succes !\n");
					}

					//printf("\n\t-->  Contact avant modification:   %s  %s  %s  %s  %s  %lld \n", resultat1.nom, resultat1.prenom, resultat1.pays,resultat1.email,resultat1.ville,resultat1.GSM);


					} while(choice_modify!=6);
					
					
					send(socketFD,&resultat1,sizeof(resultat1),0);
					int modification;
					recv(socketFD,&modification,sizeof(modification),0);
					if(modification==0){
						printf("\n\t-->  Echec de la modificatin !\n");
					}
					else{
						//printf("\n\t-->  Les modifications appliquees avec succes !\n");
					}
					
					printf("\n\t-->  Contact apres modification :   %s  %s  %s  %s  %s %s %lld \n", resultat1.nom, resultat1.prenom,resultat1.email,resultat1.pays,resultat1.ville,resultat1.rue,resultat1.GSM);
					
					}
					break;
						
				case 6 :
					printf("\n\n\n\n\n\t\tAu revoir :)\n\n\n\n\n\n\n\n\n\n");
					break;			
						
					}
			
			} while (choix_admin!=6);
			
			
		
	}
	printf("\n\t   Est que vous voulez deconnecter du serveur :  \n");
    printf("\n\t       1. Oui");
    printf("\n\t       2. Non (connecter en tant qu'un autre utilisateur')\n");
    printf("\n\t   Entrer votre choix :  ");
    scanf("%d",&quitter_serveur);
    printf("\n\n\n\n\n\n\n");
    send(socketFD,&quitter_serveur,sizeof(quitter_serveur),0);

	
}
	

    //fermure des sockets et leberation des ressurces
    closesocket(socketFD);
    WSACleanup();
    system("pause");
    return 0;
}
