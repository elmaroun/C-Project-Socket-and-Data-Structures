#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<stdbool.h>

#define LISTENING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 2
#define BUFFER_SIZE 1024

typedef struct contact {
	char nom[30],prenom[30],email[30],rue[30],ville[30],pays[30];
	long long int GSM;
} contact;

contact recherche_contact_server(char *nomfichier, char nom_a_rechercher[],char prenom_a_rechercher[]) {
    FILE *f;
    f = fopen(nomfichier, "r");
    contact c;
    bool trouv =false;

    if (f != NULL) {
        while (fscanf(f, "%s %s %lld %s %s %s %s\n", c.nom, c.prenom, &c.GSM, c.email, c.rue, c.ville, c.pays) == 7) {
        	//printf("c.nom = ",c.nom);
            if (strcmp(c.nom, nom_a_rechercher) == 0 && strcmp(c.prenom, prenom_a_rechercher) == 0 ) {
                fclose(f);
                trouv=true;
                return c; // Retourne le contact trouvé
            }
        }
        fclose(f);
    }
    if(trouv==false){
		strcpy(c.nom, "");
		return c;
		}
}



void Ajouter(char *nom_fichier, contact c) {
    FILE *f;
    f = fopen(nom_fichier, "a");
    if (f != NULL) {
        fprintf(f, "%s %s %lld %s %s %s %s \n", c.nom, c.prenom, c.GSM, c.email, c.rue, c.ville, c.pays);
    }
    fclose(f);
}



bool supprimer_contact(const char *nom_a_supprimer, const char *prenom_a_supprimer) {
    contact c;
    FILE *f = fopen("contact.txt", "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier contact.txt\n");
        return false;
    }

    FILE *temp = fopen("temp_contact.txt", "w");

    bool deleted = false;
    while (fscanf(f, "%s %s %lld %s %s %s %s", c.nom, c.prenom, &c.GSM, c.email, c.rue, c.ville, c.pays) != EOF) {
        if (strcmp(c.nom, nom_a_supprimer) == 0 && strcmp(c.prenom, prenom_a_supprimer) == 0) {
            deleted = true;
            continue;
        }
        fprintf(temp, "%s %s %lld %s %s %s %s\n", c.nom, c.prenom, c.GSM, c.email, c.rue, c.ville, c.pays);
    }

    fclose(f);
    fclose(temp);

    if (deleted) {
        if (remove("contact.txt") == 0 && rename("temp_contact.txt", "contact.txt") == 0) {
            return true;
        } else {
            printf("\nErreur lors de la suppression du contact\n");
            return false;
        }
    } else {
        remove("temp_contact.txt");
        return false;
    }
}



int main() {
	
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "(serveur) Echec d'initialisation de Winsock \n");
        exit(1);
    }
    
    //initialistin du socket
    int socketFD =socket(AF_INET, SOCK_STREAM,0);
    struct sockaddr_in socketAddress;
    socketAddress.sin_family=AF_INET;
    socketAddress.sin_port=LISTENING_PORT;
    socketAddress.sin_addr.s_addr=INADDR_ANY;
    int socketAddressLength=sizeof(socketAddress);
    
    int binReturnCode=bind(socketFD,(struct sockaddr*) &socketAddress, socketAddressLength);
    
    //attente de nouvelle connxion
    if(listen(socketFD,PENDING_QUEUE_MAXLENGTH)== -1){
        fprintf(stderr,"(serveur) echec de demarrage de l'ecoute des connexions entrantes \n");
        exit(1);
    }
    
    printf("\n\n\t***************************   SERVEUR   ***************************   \n\n\n");
    printf("\tLa socket %d est maintenant ouverte en mode TCP/IP\n\n\n", LISTENING_PORT);
    printf("     ----> Listage du port %d ...\n\n", LISTENING_PORT);
    printf("     ----> Patientez pendant que le client se connecte sur le port %d ......\n\n", LISTENING_PORT);
    
    int connectedSocketFD = accept(socketFD,(struct sockaddr*) &socketAddress,(socklen_t *) &socketAddressLength);
    int deconnecter_serveur=2;
    while(deconnecter_serveur==2){
    
    printf("\t\tUn client :) se connecte avec la socket %d de %s:%d\n\n\n", connectedSocketFD, inet_ntoa(socketAddress.sin_addr), ntohs(socketAddress.sin_port));
   
    int exist= 0;
    char log[50], type[50], mot_de_passe[50];
    
    do {
    	//reception d'un message
    	char login[20]={0};
    	recv(connectedSocketFD,login,20,0);
    	
    	printf("    - AUTHENTIFICATION -> Login : %s \n",login);
    	
    	FILE *compte;
    	compte = fopen("compte.txt", "r");
    	if (compte == NULL){
    		printf("erreur lous de l'ouverture du fichier \n");
    		exit(1);
		}
		
    	char L[100];
    	int nbr_lignes=0;
    	while(fgets(L,sizeof(L),compte)){
    		nbr_lignes++;
		}
		rewind(compte);
		
		int i=0;
   	 	while (exist==0 && fscanf(compte, "%s %s %s", log, mot_de_passe, type) == 3) {
        	if (strcmp(log, login) == 0) {
            	exist = 1;
            	printf("\t> login trouve !! \n\n");
        	}
        	i++;
    	}
	
   		 //envoi du message
    	send(connectedSocketFD,&exist,sizeof(exist),0);
    	
	} while (exist == 0);

    int mot_de_passe_correct= 0;
    int essaies=0;

    do {
    	if (essaies<3) {
    	char mot_de_passe_insere[20]={0};
    	recv(connectedSocketFD,mot_de_passe_insere,20,0);
    	
    	printf("    - AUTHENTIFICATION -> Mot de passe : %s \n",mot_de_passe_insere);
		if(strcmp(mot_de_passe_insere,mot_de_passe)== 0){
			mot_de_passe_correct=1;
			
		}
		if (mot_de_passe_correct==0){
			printf("\t> Mot de passe incorrect \n\n");
		}
		else{
			printf("\t> Mot de passe correct !!\n ");
			printf("\n\t  ---> L'utilisateur \" %s \" connecte ",log);
		}
	
   		 //envoi du message
    	send(connectedSocketFD,&mot_de_passe_correct,sizeof(mot_de_passe_correct),0);
    	essaies=essaies+1;
    }else {
    		
			printf("\n\t  ---> L'utiliateur n'a pas connecte :)\n\n\n\n\n\n\n");
			    system("pause");

    		exit(1);
		}
    	
    	
	} while(mot_de_passe_correct ==0);
	
	int admin;
	
	if(strcmp(type,"admin")==0){
		//admin
		printf("comme ADMIN \n");
		admin=1;
		send(connectedSocketFD,&admin,sizeof(admin),0);
    
		char nomfichier1[]="contact.txt";
    	int choix1;
    	do {
    		recv(connectedSocketFD,&choix1,sizeof(choix1),0);
    		
    		//le cas de rechercher
    		char nom_a_rechercher[20]="";
    		char prenom_a_rechercher[20]="";
    		int nbr_lignes_contact=0;
    		FILE*f2, *fi;
    		char L1[100], L2[100];
    		contact c3;
    		
    		switch(choix1){
    			
				case 2:
					printf("\n    - OPERATION %d -> Rechercher un contact\n",choix1);
					printf("\t> L'utilisateur a demande de rechercher une contact \n");
					
    				recv(connectedSocketFD,nom_a_rechercher,20,0);
    				printf("\n\t   - Le nom a rechercher : %s ", nom_a_rechercher);
    				
    				recv(connectedSocketFD,prenom_a_rechercher,20,0);
    				printf("\n\t   - Le prenom a rechercher : %s \n", prenom_a_rechercher);
    				
    				contact resultat;
    				resultat = recherche_contact_server(nomfichier1,nom_a_rechercher,prenom_a_rechercher);
    				
					if(strcmp(resultat.nom, "")==0) {
						printf("\n\t  ---> Contact introuvable !!\n");
					}
					else {
						printf("\n\t  ---> Contact trouve !! \n");
					}							
					send(connectedSocketFD,&resultat,sizeof(resultat),0);
    			
    				break;
    				
    			case 5:
    				
    				printf("\n    - OPERATION %d -> Afficher les contacts\n",choix1);
					printf("\t> L'utilisateur a demande d'afficher la liste des contacts \n");
					
    				fi = fopen("contact.txt", "r");
    				while(fgets(L1,sizeof(L1),fi)){
    					nbr_lignes_contact++;
						}
					fclose(fi);
					
					send(connectedSocketFD,&nbr_lignes_contact,sizeof(nbr_lignes_contact),0);
				
					int i=0;
					f2 = fopen("contact.txt", "r");
					contact c2;
					
    				while (i<nbr_lignes_contact) {
    					fscanf(f2, "%s %s %lld %s %s %s %s\n", c2.nom, c2.prenom, &c2.GSM, c2.email, c2.rue, c2.ville, c2.pays);
    					send(connectedSocketFD,&c2,sizeof(c2),0);
    					i++;
					}
					fclose(f2);
					break;
					
				case 1:
					//saisir une contact
					printf("\n    - OPERATION %d -> Ajouter un contact\n",choix1);
					printf("\t> L'utilisateur a demande d'ajouter une contact \n");
					
					recv(connectedSocketFD,&c3,sizeof(c3),0);
    			
    				printf("\n\t   - Nom : %s", c3.nom);
    				printf("\n\t   - Prenom : %s", c3.prenom);
    				printf("\n\t   - Email : %s", c3.email);
    				printf("\n\t   - Adresse : %s %s %s", c3.rue, c3.ville, c3.pays);
  					printf("\n\t   - GSM : %lld\n", c3.GSM);
  					
   	 				Ajouter("contact.txt",c3);
    				printf("\n\t  ---> Contact ajoute' avec success !!\n");
					break;
					
				case 3 :
					printf("\n    - OPERATION %d -> Supprimer un contact\n",choix1);
					printf("\t> L'utilisateur a demande de supprimer un contact \n");
					
					char nom_a_supprimer[20];
				    char prenom_a_supprimer[20];
                    recv(connectedSocketFD,nom_a_supprimer,20,0);
    			
    				printf("\n\t   - Le nom a supprimer : %s ", nom_a_supprimer);
    				recv(connectedSocketFD,prenom_a_supprimer,20,0);
    				
    				printf("\n\t   - Le prenom a supprimer : %s ", prenom_a_supprimer);
    				
    				int suppression;
                    if (supprimer_contact(nom_a_supprimer, prenom_a_supprimer)) {
                        suppression =1;
                    	send(connectedSocketFD, &suppression,sizeof(suppression) , 0);
                    	printf("\n\n\t  ---> Suppression avec success !!\n");
                    }
                    
                    else {
                        suppression = 0;
                    	send(connectedSocketFD, &suppression , sizeof(suppression) , 0);
                    	printf("\n\n\t  ---> Echec, contact introuvable pour supprimer !!\n");
                    }
                    break;
                case 4 :
					printf("\n    - OPERATION %d -> Modifier un contact\n",choix1);
					printf("\t> L'utilisateur a demande de modifier  un contact \n");
					
					char nom_a_modifier[20];
				    char prenom_a_modifier[20];
                    recv(connectedSocketFD,nom_a_modifier,20,0);
    			
    				printf("\n\t   - Le nom a modifier : %s ", nom_a_modifier);
    				recv(connectedSocketFD,prenom_a_modifier,20,0);
    				
    				printf("\n\t   - Le prenom a modifier : %s ", prenom_a_modifier);
    				contact resultat1;
    				resultat1 = recherche_contact_server(nomfichier1,nom_a_modifier,prenom_a_modifier);
    				
					if(strcmp(resultat1.nom, "")==0) {
						printf("\n\t  ---> Contact introuvable !!\n");
						send(connectedSocketFD,&resultat1,sizeof(resultat1),0);

					}
					else {
						printf("\n\t  ---> Contact trouve !! \n");
						send(connectedSocketFD,&resultat1,sizeof(resultat1),0);
						recv(connectedSocketFD,&resultat1,sizeof(resultat1),0);
    					printf("\n\t   - Nom : %s", resultat1.nom);
    					printf("\n\t   - Prenom : %s", resultat1.prenom);
    					printf("\n\t   - Email : %s", resultat1.email);
    					printf("\n\t   - Adresse : %s %s %s", resultat1.rue, resultat1.ville, resultat1.pays);
  						printf("\n\t   - GSM : %lld\n", resultat1.GSM);
  						int modification;
                    	if (supprimer_contact(nom_a_modifier, prenom_a_modifier)) {
                    		Ajouter("contact.txt",resultat1);
							modification =1;
                    		send(connectedSocketFD, &modification,sizeof(modification) , 0);
                    		printf("\n\n\t  ---> Modification avec success !!\n");
                    	}
                    
                    	else {
                        	modification = 0;
                    		send(connectedSocketFD, &modification , sizeof(modification) , 0);
                    		printf("\n\n\t  ---> Echec lors de la modification !!\n");
                    	}


					}							    				
    				
                    break;
				case 6 :
					printf("\n    - OPERATION %d -> Quitter\n",choix1);
					printf("\t> L'utilisateur a quitte \n");
					break;
					
    		}
    	} while(choix1!=6);
	}
	
	else {
		
		printf("comme INVITE \n");
		admin=0;
		char nomfichier[]="contact.txt";
		
		send(connectedSocketFD,&admin,sizeof(admin),0);
    	
    	int choix1;
    	
    	do {
    		
    	recv(connectedSocketFD,&choix1,sizeof(choix1),0);
    	
    		char nom_a_rechercher[20]="";
    		char prenom_a_rechercher[20]="";
    		int nbr_lignes_contact=0;
    		FILE*f2, *fi;
    		char L1[100], L2[100];
    		
    		switch(choix1) {
    			
				case 1:
					printf("\n    - OPERATION %d -> Rechercher un contact\n",choix1);
					printf("\t> L'utilisateur a demande de rechercher une contact \n");
					
    				recv(connectedSocketFD,nom_a_rechercher,20,0);
    				printf("\n\t   - Le nom a rechercher : %s ", nom_a_rechercher);
    				recv(connectedSocketFD,prenom_a_rechercher,20,0);
    				
    				printf("\n\t   - Le prenom a rechercher : %s \n", prenom_a_rechercher);
    				
    				contact resultat;
    				resultat = recherche_contact_server(nomfichier,nom_a_rechercher,prenom_a_rechercher);	
					send(connectedSocketFD,&resultat,sizeof(resultat),0);
    			
    				if(strcmp(resultat.nom, "")==0){
						printf("\n\t  ---> Contact introuvable !!\n");
					}
					else {
						printf("\n\t  ---> Contact trouve !! \n");
					}	
    				break;
    				
    			case 2:
    				printf("\n    - OPERATION %d -> Afficher les contacts\n",choix1);
					printf("\t> L'utilisateur a demande d'afficher la liste des contacts \n");
					
    				fi = fopen("contact.txt", "r");
    				while(fgets(L1,sizeof(L1),fi)){
    					nbr_lignes_contact++;
					}
					fclose(fi);
					
					send(connectedSocketFD,&nbr_lignes_contact,sizeof(nbr_lignes_contact),0);
					
					int i=0;
					f2 = fopen("contact.txt", "r");
					contact c2;
					
    				while(i<nbr_lignes_contact){
    					fscanf(f2, "%s %s %lld %s %s %s %s\n", c2.nom, c2.prenom, &c2.GSM, c2.email, c2.rue, c2.ville, c2.pays);
    					send(connectedSocketFD,&c2,sizeof(c2),0);
    					i++;
					}
					fclose(f2);
					break;
					
				case 3:
					printf("\n    - OPERATION %d -> Quitter\n",choix1);
					printf("\t> L'utilisateur a quitte \n");
					break;
    		}
    	} while(choix1!=3);
	}
	recv(connectedSocketFD,&deconnecter_serveur,sizeof(deconnecter_serveur),0);
	if(deconnecter_serveur==2){
		printf("\t l'utilisateur a choisi de ne pas deconnecter du serveur\n");
	}

	
}
	
    printf("\n\n\n    --> Fermeture des sockets et leberation des ressources ...\n\n\n\n\n\n\n\n");
    closesocket(connectedSocketFD);
    closesocket(socketFD);
    WSACleanup();
    system("pause");
    return 0;
}
