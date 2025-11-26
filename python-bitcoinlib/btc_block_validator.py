#!/usr/bin/env python3
"""
Bitcoin bloko hash'o validatorius
Patikrina, ar bloko hash'as yra teisingai apskaičiuotas pagal header'io informaciją
"""

from bitcoin.rpc import RawProxy
import hashlib
import struct

def reverse_bytes(hex_string):
    """
    Apverčia baitų seką (little-endian <-> big-endian konversija)
    """
    return bytes.fromhex(hex_string)[::-1].hex()

def double_sha256(data):
    """
    Atlieka dvigubą SHA256 hash'avimą (kaip Bitcoin protokole)
    """
    return hashlib.sha256(hashlib.sha256(data).digest()).digest()

def validate_block_hash(block_height_or_hash):
    """
    Patikrina bloko hash'o teisingumą
    
    Args:
        block_height_or_hash: Bloko aukštis (int) arba hash (str)
    
    Returns:
        True jei hash'as teisingas, False jei ne
    """
    try:
        # Prisijungiame prie Bitcoin Core mazgo
        p = RawProxy()
        
        # Gauname bloko hash'ą (jei gavome aukštį)
        if isinstance(block_height_or_hash, int):
            blockhash = p.getblockhash(block_height_or_hash)
            print(f"Bloko #{block_height_or_hash} hash: {blockhash}")
        else:
            blockhash = block_height_or_hash
        
        # Gauname pilną bloko informaciją
        block = p.getblock(blockhash, 2)  # Verbosity=2 grąžina pilną informaciją
        
        print("\n" + "="*70)
        print("BLOKO HEADER INFORMACIJA:")
        print("="*70)
        
        # Ištraukiame header'io laukus
        version = block['version']
        prev_block = block['previousblockhash'] if 'previousblockhash' in block else '0' * 64
        merkle_root = block['merkleroot']
        timestamp = block['time']
        bits = block['bits']
        nonce = block['nonce']
        
        print(f"Versija:           {version} (0x{version:08x})")
        print(f"Ankstesnis blokas: {prev_block}")
        print(f"Merkle Root:       {merkle_root}")
        print(f"Laikas:            {timestamp} ({block.get('mediantime', 'N/A')})")
        print(f"Bits (Difficulty): {bits}")
        print(f"Nonce:             {nonce}")
        print(f"\nBloko hash (tikrasis): {blockhash}")
        
        # Sudarome bloko header'į (80 baitų)
        # Bitcoin header'io struktūra:
        # - Version (4 bytes)
        # - Previous block hash (32 bytes)
        # - Merkle root (32 bytes)
        # - Timestamp (4 bytes)
        # - Bits (4 bytes)
        # - Nonce (4 bytes)
        
        header = b''
        
        # 1. Version (little-endian 4 bytes)
        header += struct.pack('<I', version)
        
        # 2. Previous block hash (32 bytes, reversed)
        header += bytes.fromhex(prev_block)[::-1]
        
        # 3. Merkle root (32 bytes, reversed)
        header += bytes.fromhex(merkle_root)[::-1]
        
        # 4. Timestamp (little-endian 4 bytes)
        header += struct.pack('<I', timestamp)
        
        # 5. Bits (little-endian 4 bytes)
        header += bytes.fromhex(bits)[::-1]
        
        # 6. Nonce (little-endian 4 bytes)
        header += struct.pack('<I', nonce)
        
        print(f"\nSuformuotas header (hex): {header.hex()}")
        print(f"Header'io ilgis: {len(header)} baitai")
        
        # Skaičiuojame hash'ą (double SHA256)
        calculated_hash_bytes = double_sha256(header)
        
        # Apverčiame baitus (Bitcoin naudoja little-endian)
        calculated_hash = calculated_hash_bytes[::-1].hex()
        
        print("\n" + "="*70)
        print("VALIDACIJOS REZULTATAS:")
        print("="*70)
        print(f"Apskaičiuotas hash: {calculated_hash}")
        print(f"Tikrasis hash:      {blockhash}")
        
        if calculated_hash == blockhash:
            print("\n✓ HASH'AS TEISINGAS! Blokas validus.")
            
            # Papildoma informacija apie mining difficulty
            print("\n" + "="*70)
            print("PAPILDOMA INFORMACIJA:")
            print("="*70)
            
            # Tikriname, ar hash'as atitinka difficulty reikalavimus
            hash_int = int(calculated_hash, 16)
            leading_zeros = len(calculated_hash) - len(calculated_hash.lstrip('0'))
            
            print(f"Leading zeros: {leading_zeros}")
            print(f"Hash (decimal): {hash_int}")
            print(f"Difficulty: {block.get('difficulty', 'N/A')}")
            print(f"Transakcijų skaičius: {len(block['tx'])}")
            
            return True
        else:
            print("\n✗ HASH'AS NETEISINGAS! Blokas nevalidus.")
            return False
            
    except Exception as e:
        print(f"Klaida: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    print("Bitcoin bloko hash'o validatorius")
    print("="*70)
    
    # Testuojame su keliais blokais
    test_cases = [
        0,        # Genesis blokas
        100000,   # Populiarus blokas iš užduoties
        277316,   # Alice transakcijos blokas
    ]
    
    for block_id in test_cases:
        print(f"\n{'='*70}")
        print(f"Testuojamas blokas: {block_id}")
        print('='*70)
        
        result = validate_block_hash(block_id)
        
        print("\n")
        input("Paspauskite Enter, kad tęstumėte...")

if __name__ == "__main__":
    main()