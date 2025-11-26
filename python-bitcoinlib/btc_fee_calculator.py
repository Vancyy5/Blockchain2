#!/usr/bin/env python3
"""
Bitcoin transakcijos mokesčio skaičiuoklė
Apskaičiuoja transakcijos mokestį pagal jos hash'ą
"""

from bitcoin.rpc import RawProxy
from decimal import Decimal

def calculate_transaction_fee(txid):
    """
    Apskaičiuoja Bitcoin transakcijos mokestį
    
    Mokestis = Suma visų input'ų - Suma visų output'ų
    
    Args:
        txid: Transakcijos hash (transaction ID)
    
    Returns:
        Mokestis BTC
    """
    try:
        # Prisijungiame prie Bitcoin Core mazgo
        p = RawProxy()
        
        # Gauname raw transakciją
        print(f"Gaunama transakcija: {txid}")
        raw_tx = p.getrawtransaction(txid)
        
        # Dekoduojame transakciją
        decoded_tx = p.decoderawtransaction(raw_tx)
        
        # Skaičiuojame visų output'ų sumą
        total_output = Decimal(0)
        print("\n=== OUTPUTS ===")
        for vout in decoded_tx['vout']:
            value = Decimal(str(vout['value']))
            total_output += value
            print(f"  Output {vout['n']}: {value} BTC")
        
        print(f"\nBendra output'ų suma: {total_output} BTC")
        
        # Skaičiuojame visų input'ų sumą
        # Reikia gauti kiekvieno input'o pradinę transakciją
        total_input = Decimal(0)
        print("\n=== INPUTS ===")
        
        for vin in decoded_tx['vin']:
            # Coinbase transakcijos neturi įprastų input'ų
            if 'coinbase' in vin:
                print("  Tai coinbase transakcija (nauji bitcoinai)")
                return Decimal(0)  # Coinbase transakcijos neturi mokesčio
            
            # Gauname input'o pradinę transakciją
            prev_txid = vin['txid']
            prev_vout = vin['vout']
            
            # Gauname pradinę transakciją
            prev_raw_tx = p.getrawtransaction(prev_txid)
            prev_decoded_tx = p.decoderawtransaction(prev_raw_tx)
            
            # Randame konkretų output'ą, kuris dabar yra input'as
            prev_output = prev_decoded_tx['vout'][prev_vout]
            input_value = Decimal(str(prev_output['value']))
            total_input += input_value
            
            print(f"  Input iš {prev_txid[:16]}...:{prev_vout} = {input_value} BTC")
        
        print(f"\nBendra input'ų suma: {total_input} BTC")
        
        # Skaičiuojame mokestį
        fee = total_input - total_output
        
        print("\n" + "="*50)
        print(f"TRANSAKCIJOS MOKESTIS: {fee} BTC")
        
        # Papildoma informacija
        if 'vsize' in decoded_tx:
            vsize = decoded_tx['vsize']
            fee_per_byte = (fee / Decimal(vsize)) * Decimal(100000000)  # satoshi per byte
            print(f"Transakcijos dydis: {vsize} vbytes")
            print(f"Mokestis už bytą: {fee_per_byte:.2f} satoshi/vbyte")
        
        print("="*50)
        
        return fee
        
    except Exception as e:
        print(f"Klaida: {e}")
        return None

def main():
    # 2019-09-06 viena vertingiausių transakcijų
    # Ši transakcija perkėlė ~94,504 BTC su 0.5 BTC mokesčiu
    expensive_tx = "4410c8d14ff9f87ceeed1d65cb58e7c7b2422b2d7529afc675208ce2ce09ed7d"
    
    print("Bitcoin transakcijos mokesčio analizė")
    print("="*50)
    
    # Apskaičiuojame mokestį
    fee = calculate_transaction_fee(expensive_tx)
    
    if fee:
        # Konvertuojame į USD (orientacinis kursas 2019-09-06: ~$10,500)
        btc_price_2019 = 10500
        fee_usd = float(fee) * btc_price_2019
        print(f"\nMokestis USD (2019-09-06 kursu): ${fee_usd:,.2f}")
        print(f"Mokestis satoshi: {float(fee) * 100000000:,.0f} sat")

if __name__ == "__main__":
    main()